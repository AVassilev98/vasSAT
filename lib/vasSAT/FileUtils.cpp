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
  std::string line;
  ifs.open(path);
  if (ifs.is_open()) {
    while (!ifs.eof()) {
      std::getline(ifs, line);
      // skip initial comments and p line
      if (line[0] == 'c' || line[0] == 'p') continue;

      std::vector<int> clause;
      for (unsigned i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') continue;
        if (line[i] == '0')
          std::cout << "Variables must start at one!: " << path << std::endl;
        if (i == 0 || line[i - 1] == ' ') {
          string::size_type sz = i;
          int var = stoi(line, &sz);

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