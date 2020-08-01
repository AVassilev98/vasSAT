#pragma once
#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/NNFFormula.hpp"

#include <memory>
#include <string>

namespace vasSAT {
using CNFRef = std::unique_ptr<CNFFormula>;
using NNFRef = std::unique_ptr<NNFFormula>;

class Parser {
private:
  void parseNNFString(const std::string &str, unsigned endIdx,
                      NNFRef &nnf) const;

public:
  CNFRef parseCNFFile(const std::string &path) const;
  NNFRef parseNNfFile(const std::string &path) const;
};
} // namespace vasSAT