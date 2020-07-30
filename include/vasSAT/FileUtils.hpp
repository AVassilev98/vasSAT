#pragma once
#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/NNFFormula.hpp"

#include <memory>
#include <string>

namespace vasSAT {
using CNFRef = std::shared_ptr<vasSAT::CNFFormula>;

class Parser {
public:
  CNFRef parseCNFFile(const std::string &path) const;
};
} // namespace vasSAT