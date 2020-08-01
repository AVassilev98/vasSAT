#pragma once

#include <iostream>
#include <list>
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
  // maps height to a vector of Nodes at that height
  std::unordered_map<unsigned, std::list<NodeRef>> m_heightMap;

  NNFFormula() = default;

  bool isValid() const;

  std::string inorder(const NodeRef &node) const;
  void populateHeightMap();
  void mergeNodes();

public:
  void printExternalToInternal(std::ostream &os) const;
  void print(std::ostream &os) const;
};
} // namespace vasSAT