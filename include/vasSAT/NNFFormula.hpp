#pragma once

#include <iostream>
#include <optional>
#include <unordered_map>

namespace vasSAT {

class Parser;

struct NNFData {
  std::optional<unsigned> externalID;
  std::optional<unsigned> leftChild;
  std::optional<unsigned> rightChild;
};

class NNFFormula {
private:
  NNFFormula();

  // each node will be inserted by its internal ID
  std::unordered_map<unsigned, NNFData> m_nodes;
  unsigned m_id = 0;
  unsigned m_rootID = 0;

  void checkNoCycles() const;
  void checkTerminates() const;

  // returns the ID of the node you add
  unsigned addNode(std::optional<unsigned> externalID);
  void setLeft(unsigned curID, unsigned leftId);
  void setRight(unsigned curID, unsigned rightID);

public:
  friend class Parser;

  void printExternalToInternal(std::ostream &os) const;
};
} // namespace vasSAT