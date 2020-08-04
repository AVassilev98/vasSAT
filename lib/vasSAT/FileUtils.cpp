#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <unordered_set>
#include <vector>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/FileUtils.hpp"
#include "vasSAT/NNFFormula.hpp"
#include "vasSAT/Node.hpp"

using NNFRef = std::unique_ptr<vasSAT::NNFFormula>;

namespace {
bool validParenths(const std::string &str) {
  std::stack<char> parenthStack;

  for (char c : str) {
    switch (c) {
    case '(':
      parenthStack.push('(');
      break;

    case ')':
      if (parenthStack.top() == '(') {
        parenthStack.pop();
      } else {
        return false;
      }
      break;

    default:
      break;
    }
  }

  return parenthStack.empty();
}

bool unknownSymbols(const std::string &str) {
  // valid symbols : '-', '.', '+', '(', ')', unsigned integers

  std::unordered_set<char> symbols = {' ', '-', '.', '+', '(', ')', '1', '2',
                                      '3', '4', '5', '6', '7', '8', '9'};

  for (char c : str) {
    if (c == '0') return false;
    if (symbols.find(c) == symbols.end()) return true;
  }

  return false;
}

bool multipleEndTags(const std::string &str) {
  // valid symbols : '-', '.', '+', '(', ')', unsigned integers

  bool seenTag = false;

  for (char c : str) {
    if (c == '0') {
      if (seenTag) return true;
      seenTag = true;
    }
  }

  return false;
}

} // namespace

namespace vasSAT {

CNFRef Parser::parseCNFFile(const std::string &path) const {

  using namespace std;

  auto formula = make_unique<vasSAT::CNFFormula>();

  ifstream ifs;
  std::string cnfLine;
  ifs.open(path);
  if (ifs.is_open()) {
    while (!ifs.eof()) {
      getline(ifs, cnfLine);
      // skip initial comments and p line
      if (cnfLine[0] == 'c' || cnfLine[0] == 'p') continue;

      std::vector<int> clause;
      for (unsigned i = 0; i < cnfLine.size(); ++i) {
        if (cnfLine[i] == ' ') continue;
        if (cnfLine[i] == '0' && i == cnfLine.size() - 1) continue;
        if (cnfLine[i] == '0' && i != cnfLine.size() - 1) {
          std::cerr << "Variables must start at 1: " << path << std::endl;
          throw new std::invalid_argument("invalid end tag");
        }
        if (i == 0 || cnfLine[i - 1] == ' ') {
          int var = atoi(&cnfLine.c_str()[i]);

          if (var == 0) {
            std::cerr << "Unkown symbol found: " << path << std::endl;
            throw new std::invalid_argument("unknown symbol");
          } else
            clause.push_back(var);
        }
      }

      formula->addClause(clause);
    }

    formula->initAsgnMap();
    return formula;

  } else {
    std::cerr << "Could not open file: " << path << std::endl;
    throw new std::invalid_argument("Could not open file");
  }
}

NNFRef Parser::parseNNfFile(const std::string &path) const {
  using namespace std;

  NNFRef ret(new NNFFormula());
  ifstream ifs;
  std::string nnfLine;
  ifs.open(path);
  if (ifs.is_open()) {
    std::getline(ifs, nnfLine);
    if (!ifs.eof())
      std::cout << "Detected multiple lines, ignoring past first\n";

    nnfLine.erase(std::remove(nnfLine.begin(), nnfLine.end(), ' '),
                  nnfLine.end());

    unsigned endIdx = nnfLine.size() - 1;
    while (nnfLine[endIdx] != '0' && endIdx > 0) {
      endIdx--;
    }
    if (endIdx == 0) {
      std::cerr << "No end tag detected. Insert '0' at end of nnf equation\n";
      throw new std::invalid_argument("No end tag detected");
    }

    if (!validParenths(nnfLine)) {
      std::cerr << "Unable to read equation due to invalid parentheses\n";
      throw new std::invalid_argument("invalid parenths");
    }

    if (unknownSymbols(nnfLine)) {
      std::cerr << "Unable to read equation due to unknown symbols detected\n";
      throw new std::invalid_argument("unknown symbols");
    }

    if (multipleEndTags(nnfLine)) {
      std::cerr << "Multiple '0' characters. This character is reserved as an "
                   "end tag\n";
      throw new std::invalid_argument("multiple end tags");
    }

    parseNNFString(nnfLine, endIdx, ret);

  } else {
    std::cerr << "Could not open file: " << path << std::endl;
    throw new std::invalid_argument("Could not open file");
  }

  if (!ret->isValid()) {
    std::cerr << "Poorly formed NNF equation\n";
    throw new invalid_argument("Not NNF");
  }

  ret->populateHeightMap();
  ret->mergeNodes();

  return ret;
}

void Parser::parseNNFString(const std::string &str, unsigned endIdx,
                            NNFRef &nnf) const {

  auto isNumber = [](char c) -> bool { return 0 < c - '0' && c - '0' <= 9; };

  // We perform the shunting-yard algorithm to change to polish notation and
  // eliminate extra parentheses. Read more here:
  // https://en.wikipedia.org/wiki/Shunting-yard_algorithm

  // precedence from greatest to least
  std::unordered_map<char, unsigned> precedenceMap = {
      {'-', 2}, {'.', 1}, {'+', 0}};

  std::stack<std::string> PN;
  std::stack<char> ops;

  auto shunting = [&]() {
    for (int i = endIdx - 1; i >= 0; i--) {
      char c = str[i];

      if (isNumber(c)) {
        std::string number;
        number.push_back(c);
        for (; i > 0 && isNumber(str[i - 1]); i--) {
          number.push_back(str[i - 1]);
        }
        std::reverse(number.begin(), number.end());
        PN.push(number);
      } else if (c == ')') {
        ops.push(')');
      } else if (c == '(') {
        while (ops.top() != ')') {
          PN.push(std::string(1, ops.top()));
          ops.pop();
        }
        ops.pop();
      } else {
        while (!ops.empty() &&
               (precedenceMap.find(ops.top()) != precedenceMap.end()) &&
               precedenceMap.find(ops.top())->second >
                   precedenceMap.find(c)->second) {
          PN.push(std::string(1, ops.top()));
          ops.pop();
        }
        ops.push(c);
      }
    }

    while (!ops.empty()) {
      PN.push(std::string(1, ops.top()));
      ops.pop();
    }
  };

  shunting();

  std::unordered_map<char, NodeType> typeMap = {
      {'-', NodeType::NOT}, {'+', NodeType::OR},  {'.', NodeType::AND},
      {'1', NodeType::LIT}, {'2', NodeType::LIT}, {'3', NodeType::LIT},
      {'4', NodeType::LIT}, {'5', NodeType::LIT}, {'6', NodeType::LIT},
      {'7', NodeType::LIT}, {'8', NodeType::LIT}, {'9', NodeType::LIT}};

  if (PN.empty()) return;

  std::stack<NodeRef> treeStack;
  // build tree
  int internalID = 0;
  while (!PN.empty()) {
    // check if first char of string is a symbol
    if (typeMap.find(PN.top()[0]) == typeMap.end())
      assert(0 && "Unknown symbol during node creation!\n");

    switch (typeMap.find(PN.top()[0])->second) {
    case NodeType::AND:
      treeStack.push(std::make_shared<AndNode>(internalID));
      break;
    case NodeType::OR:
      treeStack.push(std::make_shared<OrNode>(internalID));
      break;
    case NodeType::NOT:
      treeStack.push(std::make_shared<NotNode>(internalID));
      break;
    case NodeType::LIT: {
      int externalID = atoi(PN.top().c_str());
      treeStack.push(std::make_shared<LitNode>(internalID, externalID));
      break;
    }
    default:
      assert(0 && "Expression type not handled by switch!\n");
      break;
    }
    PN.pop();
    internalID++;
  }

  std::stack<NodeRef> valStack;
  // connect tree
  while (!treeStack.empty()) {
    auto elem = treeStack.top();
    treeStack.pop();

    switch (elem->getType()) {
    case NodeType::AND:
    case NodeType::OR:
      elem->insertLeft(valStack.top());
      valStack.pop();
      elem->insertRight(valStack.top());
      valStack.pop();
      break;
    case NodeType::NOT:
      elem->insertRight(valStack.top());
      valStack.pop();
      break;
    case NodeType::LIT:
      break;
    default:
      assert(0 && "Expression type not handled by switch!\n");
      break;
    }
    valStack.push(elem);
  }
  nnf->m_rootNode = valStack.top();
}
} // namespace vasSAT
