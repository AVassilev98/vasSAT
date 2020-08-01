#include <assert.h>
#include <stack>
#include <unordered_set>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/NNFFormula.hpp"
#include "vasSAT/Node.hpp"

namespace vasSAT {

void NNFFormula::checkNoCycles() const {
  std::unordered_set<NodeRef> visited;
  std::stack<NodeRef> toVisit;

  toVisit.push(m_rootNode);

  while (!toVisit.empty()) {
    auto &node = toVisit.top();
    assert(visited.find(node) == visited.end() && "Cycle Detected!");

    visited.insert(node);
    toVisit.pop();

    if (node->getLeft().has_value()) toVisit.push(node->getLeft().value());
    if (node->getRight().has_value()) toVisit.push(node->getRight().value());
  }
}

void NNFFormula::printExternalToInternal(std::ostream &os) const {
  std::stack<NodeRef> toVisit;

  toVisit.push(m_rootNode);
  while (!toVisit.empty()) {
    auto &ref = toVisit.top();

    if (ref->getType() == NodeType::LIT) {
      const auto &litNode = std::dynamic_pointer_cast<LitNode>(ref);
      int externalID = litNode->getExternalID();
      int internalID = litNode->getID();

      os << externalID << " ---> " << internalID << "\n";
    }

    if (ref->getLeft().has_value()) { toVisit.push(ref->getLeft().value()); }
    if (ref->getRight().has_value()) { toVisit.push(ref->getRight().value()); }
  }
  os.flush();
}

std::string NNFFormula::inorder(const NodeRef &node) const {
  auto type = node->getType();
  std::string str = "";

  bool binaryNode =
      type == NodeType::AND || type == NodeType::OR || type == NodeType::NOT;

  if (binaryNode) str = "(" + str;
  if (node->getLeft().has_value()) str += inorder(node->getLeft().value());

  switch (type) {
  case NodeType::LIT: {
    const auto &litNode = std::dynamic_pointer_cast<LitNode>(node);
    str.append(std::to_string(litNode->getExternalID()));
    break;
  }
  case NodeType::AND:
    str.append(".");
    break;
  case NodeType::OR:
    str.append("+");
    break;
  case NodeType::NOT:
    str.append("-");
    break;
  default:
    assert(0 && "Attempted to print unknown node type!");
  }

  if (node->getRight().has_value()) str += inorder(node->getRight().value());

  if (binaryNode) str = str + ")";

  return str;
}

void NNFFormula::print(std::ostream &os) const {
  std::string str = inorder(m_rootNode);
  os << str << "0" << std::endl;
}

class CNFDispatcher : public AbstractNodeDispatcher {
private:
  CNFFormula &m_formula;

  // because the nodes are merged it is possible to visit the same NodeData more
  // than once, in which case we don't want to add duplicate clauses to our CNF
  std::unordered_set<NodeDataRef> m_visited;

public:
  CNFDispatcher(CNFFormula &F) : m_formula(F) {}

  void Dispatch(AndNode &N) override {
    if (m_visited.find(N.getData()) != m_visited.end()) return;
    m_visited.insert(N.getData());

    int curID = (int)N.getID();
    int leftID = (int)N.getLeft().value()->getID();
    int rightID = (int)N.getLeft().value()->getID();

    m_formula.addClause({curID, -leftID, -rightID});
    m_formula.addClause({-curID, leftID});
    m_formula.addClause({-curID, rightID});

    N.getLeft().value()->Accept(*this);
    N.getRight().value()->Accept(*this);
  }
  void Dispatch(OrNode &N) override {
    if (m_visited.find(N.getData()) != m_visited.end()) return;
    m_visited.insert(N.getData());

    int curID = (int)N.getID();
    int leftID = (int)N.getLeft().value()->getID();
    int rightID = (int)N.getLeft().value()->getID();

    m_formula.addClause({-curID, leftID, rightID});
    m_formula.addClause({curID, -leftID});
    m_formula.addClause({curID, -rightID});

    N.getLeft().value()->Accept(*this);
    N.getRight().value()->Accept(*this);
  }
  void Dispatch(NotNode &N) override {
    if (m_visited.find(N.getData()) != m_visited.end()) return;
    m_visited.insert(N.getData());

    int curID = (int)N.getID();
    int rightID = (int)N.getLeft().value()->getID();

    m_formula.addClause({-curID, -rightID});
    m_formula.addClause({curID, rightID});

    N.getRight().value()->Accept(*this);
  }
  void Dispatch(LitNode &N) override { return; }
};

} // namespace vasSAT