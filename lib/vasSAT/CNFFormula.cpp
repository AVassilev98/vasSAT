#include <cmath>
#include <unordered_map>
#include <vector>

#include "vasSAT/CNFFormula.hpp"

namespace vasSAT {
using namespace std;

void CNFFormula::addClause(const std::vector<int> &lits) {
  std::vector<unsigned> clause;
  for (int lit : lits) {
    bool negation = lit < 0;
    m_vars[lit * (negation * -1)] = m_id;
    if (negation) clause.push_back(m_id * 2 + 1);
    else
      clause.push_back(m_id * 2);

    ++m_id;
  }

  m_clauses.push_back(clause);
}

} // namespace vasSAT