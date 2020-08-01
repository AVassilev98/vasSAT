#pragma once

#include <iostream>
#include <optional>
#include <unordered_map>

#include "vasSAT/Node.hpp"

namespace vasSAT {

class Parser;
enum class NodeType;

class NNFFormula {
private:
  NNFFormula() = default;

  // each node will be inserted by its internal ID
  NodeRef m_rootNode;

  void checkNoCycles() const;
  void checkTerminates() const;

  void inorder(const NodeRef &node, std::string &str) const;

  // returns the ID of the node you add
  unsigned addNode(std::optional<unsigned> externalID, NodeType type);
  void setLeft(unsigned curID, unsigned leftId);
  void setRight(unsigned curID, unsigned rightID);

public:
  friend class Parser;

  void printExternalToInternal(std::ostream &os) const;
  void print(std::ostream &os) const;
};
} // namespace vasSAT