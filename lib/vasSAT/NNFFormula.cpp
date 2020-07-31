#include <assert.h>
#include <stack>
#include <unordered_set>

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

} // namespace vasSAT