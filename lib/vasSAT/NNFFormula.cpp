#include <assert.h>
#include <stack>
#include <unordered_set>

#include "vasSAT/NNFFormula.hpp"

namespace vasSAT {
unsigned NNFFormula::addNode(std::optional<unsigned> externalID) {
  m_nodes.insert({m_id, {externalID}});
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

} // namespace vasSAT