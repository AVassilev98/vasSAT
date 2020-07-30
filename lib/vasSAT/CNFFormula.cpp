#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "vasSAT/CNFFormula.hpp"

namespace vasSAT {
using namespace std;

void CNFFormula::addClause(const std::vector<int> &lits) {
  std::vector<unsigned> clause;
  for (int lit : lits) {

    if (m_vars.find(abs(lit)) != m_vars.end()) {
      clause.push_back(m_vars[abs(lit)] * 2);
    } else {

      bool negation = lit < 0;
      m_vars[abs(lit)] = m_id;
      if (negation) clause.push_back(m_id * 2 + 1);
      else
        clause.push_back(m_id * 2);

      ++m_id;
    }
  }

  m_clauses.push_back(clause);
}

void CNFFormula::print(std::ostream &os) const {
  auto externalID = [](unsigned i) { return i + 1; };

  // print DIMACS Header
  os << "p cnf " << m_vars.size() << " " << m_clauses.size() << "\n";

  // print clauses
  for (auto &clause : m_clauses) {
    for (int var : clause) {
      os << externalID(var) << " ";
    }
    os << "0\n";
  }
}

void CNFFormula::printToFile(std::string str) const {
  ofstream ofs(str);
  if (ofs.is_open()) print(ofs);
  else {
    std::cout << "Unable to open destination file: " << str << endl;
  }
}
} // namespace vasSAT