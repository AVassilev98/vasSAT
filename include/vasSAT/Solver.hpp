#pragma once
#include <stack>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/Node.hpp"

namespace vasSAT {
enum class Assignment;

class Solver {
  struct DecisionData;
  using VarToClauseMap = std::unordered_map<int, std::vector<Clause *>>;

private:
  void initVtcMap(VarToClauseMap &vtcMap,
                  const std::unique_ptr<CNFFormula> &F) const;

  bool DPLL(VarToClauseMap &vtcMap, std::unique_ptr<CNFFormula> &F) const;

public:
  bool Solve(std::unique_ptr<CNFFormula> &F) const;
};
} // namespace vasSAT