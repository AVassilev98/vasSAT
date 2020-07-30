#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/FileUtils.hpp"

std::shared_ptr<vasSAT::CNFFormula> parseCNFFile(const std::string &path) {

  using namespace std;

  auto formula = make_shared<vasSAT::CNFFormula>();

  ifstream ifs;
  std::string cnfLine;
  ifs.open(path);
  if (ifs.is_open()) {
    while (!ifs.eof()) {
      getline(ifs, cnfLine);
      // skip initial comments and p line
      if (cnfLine[0] == 'c' || cnfLine[0] == 'p') continue;

      std::vector<int> clause;
      for (unsigned i = 0; i < cnfLine.size(); ++i) {
        if (cnfLine[i] == ' ') continue;
        if (cnfLine[i] == '0' && i == cnfLine.size() - 1) continue;
        if (cnfLine[i] == '0' && i != cnfLine.size() - 1)
          std::cout << "Variables must start at one!: " << path << std::endl;
        if (i == 0 || cnfLine[i - 1] == ' ') {
          int var = atoi(&cnfLine.c_str()[i]);

          if (var == 0)
            std::cout << "Unkown symbol found!: " << path << std::endl;
          else
            clause.push_back(var);
        }
      }

      formula->addClause(clause);
    }

    return formula;

  } else {
    std::cout << "Could not read file: " << path << std::endl;
    return formula;
  }
}