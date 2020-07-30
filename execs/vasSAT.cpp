#include "cxxopts/cxxopts.hpp"
#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/DAG.hpp"

int main(int argc, char *argv[]) {
  vasSAT::CNFFormula cnf;
  cnf.addClause({1, 3, 5});
  cnf.addClause({2, 3, 6});
  cnf.print(std::cout);

  return 0;
}