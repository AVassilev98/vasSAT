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
  friend class Parser;
  NodeRef m_rootNode = nullptr;

  NNFFormula() = default;

  // each node will be inserted by its internal ID

  void checkNoCycles() const;
  void checkTerminates() const;

  void inorder(const NodeRef &node, std::string &str) const;

public:
  void printExternalToInternal(std::ostream &os) const;
  void print(std::ostream &os) const;
};
} // namespace vasSAT