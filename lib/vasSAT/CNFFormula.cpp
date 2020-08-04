#include <cmath>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "vasSAT/CNFFormula.hpp"

namespace vasSAT {
using namespace std;

void CNFFormula::addClause(const std::vector<int> &lits) {
  Clause clause;
  for (int lit : lits) {

    if (m_vars.find(abs(lit)) == m_vars.end()) {
      m_asgnMap.insert({m_id, Assignment::Empty});
      m_vars.insert({abs(lit), m_id});
      ++m_id;
    }

    unsigned id = m_vars.find(abs(lit))->second;
    bool negation = lit < 0;
    clause.push_back(id * 2 + negation);
  }

  m_clauses.push_back(clause);
}

void CNFFormula::print(std::ostream &os) const {

  // print DIMACS Header
  os << "p cnf " << m_vars.size() << " " << m_clauses.size() << "\n";

  // print clauses
  for (auto &clause : m_clauses) {
    for (int var : clause) {
      for (auto &pair : m_vars) {
        if (pair.second * 2 == var) os << pair.first << " ";
        else if (pair.second * 2 + 1 == var)
          os << "-" << pair.first << " ";
      }
    }
    os << "0\n";
  }
}

void CNFFormula::printToFile(std::string &str) const {
  ofstream ofs(str);
  if (ofs.is_open()) print(ofs);
  else {
    std::cerr << "Unable to open destination file: " << str << endl;
    throw new invalid_argument("Could not open file");
  }
}

void CNFFormula::printAssignment(std::ostream &os) const {
  for (auto &var : m_vars) {
    auto asgn = m_asgnMap.find(var.second)->second;
    std::string asgnStr = "";
    switch (asgn) {
    case Assignment::True:
      asgnStr = "1";
      break;
    case Assignment::False:
      asgnStr = "0";
      break;
    default:
      asgnStr = "<EMPTY>";
      break;
    }

    os << var.first << ": " << asgnStr << "\n";
  }
  os.flush();
}

void CNFFormula::printAssignmentToFile(std::string &str) const {
  ofstream ofs(str);
  if (ofs.is_open()) printAssignment(ofs);
  else {
    std::cerr << "Unable to open destination file: " << str << endl;
    throw new invalid_argument("Could not open file");
  }
}

} // namespace vasSAT