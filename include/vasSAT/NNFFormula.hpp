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
  std::unordered_map<int, NodeRef> m_heightMap;

  NNFFormula() = default;

  bool isValid() const;

  std::string inorder(const NodeRef &node) const;

public:
  void printExternalToInternal(std::ostream &os) const;
  void print(std::ostream &os) const;
};
} // namespace vasSAT