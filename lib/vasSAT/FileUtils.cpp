#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/FileUtils.hpp"
#include "vasSAT/NNFFormula.hpp"

namespace vasSAT {

CNFRef Parser::parseCNFFile(const std::string &path) const {

  using namespace std;

  auto formula = make_unique<vasSAT::CNFFormula>();

  ifstream ifs;
  std::string line;
  ifs.open(path);
  if (ifs.is_open()) {
    while (!ifs.eof()) {
      std::getline(ifs, line);
      // skip initial comments and p line
      if (line[0] == 'c' || line[0] == 'p') continue;

      std::vector<int> clause;
      for (unsigned i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') continue;
        if (line[i] == '0')
          throw new invalid_argument("Variables must start at one!: " + path);
        if (i == 0 || line[i - 1] == ' ') {
          string::size_type sz = i;
          int var = stoi(line, &sz);

          if (var == 0)
            throw new invalid_argument("Unkown symbol found!: " + path);
          else
            clause.push_back(var);
        }
      }

      formula->addClause(clause);
    }

    return formula;

  } else {
    throw new invalid_argument("Could not open file: " + path);
  }
}

NNFRef Parser::parseNNfFile(const std::string &path) const {
  auto f = new NNFFormula();
  std::unique_ptr<NNFFormula> ret;
  ret.reset(f);

  return ret;
}
} // namespace vasSAT