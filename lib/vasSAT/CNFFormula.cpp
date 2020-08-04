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

void CNFFormula::initAsgnMap() {
  for (auto var : m_vars) {
    m_asgnMap.insert({var.second, Assignment::Empty});
  }
}
} // namespace vasSAT