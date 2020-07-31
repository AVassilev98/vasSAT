#include <assert.h>
#include <stack>
#include <unordered_set>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/NNFFormula.hpp"
#include "vasSAT/Node.hpp"

namespace vasSAT {
unsigned NNFFormula::addNode(std::optional<unsigned> externalID,
                             NodeType type) {
  m_nodes.insert({m_id, {type, externalID}});
  ++m_id;
  return m_id - 1;
}

void NNFFormula::setLeft(unsigned curID, unsigned leftId) {
  assert(m_nodes.find(curID) != m_nodes.end() && "Current node doesn't exist!");
  assert(!m_nodes.at(curID).leftChild.has_value() &&
         "Already has a left child!");

  m_nodes.at(curID).leftChild = leftId;
}

void NNFFormula::setRight(unsigned curID, unsigned rightID) {
  assert(m_nodes.find(curID) != m_nodes.end() && "Current node doesn't exist!");
  assert(!m_nodes.at(curID).rightChild.has_value() &&
         "Already has a right child!");

  m_nodes.at(curID).rightChild = rightID;
}

void NNFFormula::checkNoCycles() const {
  std::unordered_set<unsigned> visited;
  std::stack<unsigned> toVisit;

  toVisit.push(m_rootID);

  while (!toVisit.empty()) {
    unsigned id = toVisit.top();
    assert(visited.find(id) == visited.end() && "Cycle Detected!");

    visited.insert(id);
    toVisit.pop();

    if (m_nodes.find(id)->second.leftChild.has_value())
      toVisit.push(m_nodes.find(id)->second.leftChild.value());
    if (m_nodes.find(id)->second.rightChild.has_value())
      toVisit.push(m_nodes.find(id)->second.rightChild.value());
  }

  assert(visited.size() == m_nodes.size() &&
         "Nodes exist outside of main tree!\n");
}

void NNFFormula::printExternalToInternal(std::ostream &os) const {
  for (auto &elem : m_nodes) {
    if (elem.second.externalID.has_value())
      os << elem.second.externalID.value() << " ---> " << elem.first << "\n";
  }
  os.flush();
}

void NNFFormula::inorder(unsigned elem, std::string &str) const {
  auto &data = m_nodes.find(elem)->second;
  bool binaryNode = data.type == NodeType::AND || data.type == NodeType::OR;

  if (data.leftChild.has_value()) { inorder(data.leftChild.value(), str); }
  if (binaryNode) str = "(" + str + ")";

  switch (data.type) {
  case NodeType::LIT:
    str.append(std::to_string(data.externalID.value()));
    break;
  case NodeType::AND:
    str.append(" . ");
    break;
  case NodeType::OR:
    str.append(" + ");
    break;
  case NodeType::NOT:
    str.append(" -");
    break;
  default:
    assert(0 && "Attempted to print unknown node type!");
  }

  if (data.rightChild.has_value()) { inorder(data.rightChild.value(), str); }

  if (binaryNode) str = "(" + str + ")";
}

void NNFFormula::print(std::ostream &os) const {
  std::string str;
  inorder(m_rootID, str);

  os << str << std::endl;
}

class CNFDispatcher : public AbstractNodeDispatcher {
private:
  CNFFormula &m_formula;

  // because the nodes are merged it is possible to visit the same NodeData more
  // than once, in which case we don't want to add duplicate clauses to our CNF
  std::unordered_set<NodeDataRef> m_visited;

public:
  CNFDispatcher(CNFFormula &F) : m_formula(F) {}
  void Dispatch(AndNode &N) override;
  void Dispatch(OrNode &N) override;
  void Dispatch(NotNode &N) override;
  void Dispatch(LitNode &N) override;
};

void CNFDispatcher::Dispatch(AndNode &N) {
  if (m_visited.find(N.getData()) != m_visited.end()) return;
  m_visited.insert(N.getData());

  int curID = (int)N.getID();
  int leftID = (int)N.getLeft()->getID();
  int rightID = (int)N.getLeft()->getID();

  m_formula.addClause({curID, -leftID, -rightID});
  m_formula.addClause({-curID, leftID});
  m_formula.addClause({-curID, rightID});

  N.getLeft()->Accept(*this);
  N.getRight()->Accept(*this);
}
void CNFDispatcher::Dispatch(OrNode &N) {
  if (m_visited.find(N.getData()) != m_visited.end()) return;
  m_visited.insert(N.getData());

  int curID = (int)N.getID();
  int leftID = (int)N.getLeft()->getID();
  int rightID = (int)N.getLeft()->getID();

  m_formula.addClause({-curID, leftID, rightID});
  m_formula.addClause({curID, -leftID});
  m_formula.addClause({curID, -rightID});

  N.getLeft()->Accept(*this);
  N.getRight()->Accept(*this);
}
void CNFDispatcher::Dispatch(NotNode &N) {
  if (m_visited.find(N.getData()) != m_visited.end()) return;
  m_visited.insert(N.getData());

  int curID = (int)N.getID();
  int leftID = (int)N.getLeft()->getID();
  int rightID = (int)N.getLeft()->getID();

  m_formula.addClause({curID, -leftID, -rightID});
  m_formula.addClause({-curID, leftID});
  m_formula.addClause({-curID, rightID});

  N.getRight()->Accept(*this);
}
void CNFDispatcher::Dispatch(LitNode &N) { return; }

} // namespace vasSAT