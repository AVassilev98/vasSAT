#pragma once
#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/NNFFormula.hpp"

#include <memory>
#include <string>

namespace vasSAT {
using CNFRef = std::unique_ptr<CNFFormula>;
using NNFRef = std::unique_ptr<NNFFormula>;

class Parser {
public:
  CNFRef parseCNFFile(const std::string &path) const;
  NNFRef parseNNfFile(const std::string &path) const;
};
} // namespace vasSAT