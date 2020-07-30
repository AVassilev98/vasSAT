#pragma once

#include <unordered_map>
#include <vector>

namespace vasSAT {

class CNFFormula {
  int m_id = 0;
  std::vector<std::vector<unsigned>> m_clauses;
  std::unordered_map<unsigned, unsigned> m_vars;

public:
  CNFFormula() = default;
  ~CNFFormula() = default;
  CNFFormula(const CNFFormula &rhs) = delete;
  CNFFormula(CNFFormula &&rhs) = delete;
  CNFFormula &operator=(const CNFFormula &rhs) = delete;
  bool operator==(const CNFFormula &rhs) const = delete;

  void addClause(const std::vector<int> &lits);
  void addVar(int var);
  bool hasVar(int var);
  bool hasLit(int lit);
};

} // namespace vasSAT