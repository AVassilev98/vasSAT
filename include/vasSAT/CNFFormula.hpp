#pragma once

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace vasSAT {
using Clause = std::vector<unsigned>;

class CNFFormula {
  enum class Assignment { Empty, True, False };
  using AssignmentMap = std::unordered_map<unsigned, Assignment>;

  unsigned m_id = 0;
  std::vector<Clause> m_clauses;
  std::unordered_map<unsigned, unsigned> m_vars;
  AssignmentMap m_asgnMap;

  void initAsgnMap();

public:
  friend class Solver;
  friend class Parser;

  void addClause(const std::vector<int> &lits);

  void printAssignment(std::ostream &os) const;
  void printAssignmentToFile(std::string &str) const;
  void print(std::ostream &os) const;
  void printToFile(std::string &str) const;
};

} // namespace vasSAT