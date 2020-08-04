#include <exception>
#include <fstream>

#include "cxxopts/cxxopts.hpp"
#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/FileUtils.hpp"
#include "vasSAT/Solver.hpp"

static cxxopts::Options options("vasSAT", "A classical DPLL Sat Solver");

int main(int argc, char *argv[]) {
  using namespace std;
  using StringList = vector<string>;

  // clang-format off
  options.add_options()
  ("n,nnfFile", "NNF equation file path",cxxopts::value<StringList>())
  ("c,cnfFile", "CNF equation file path", cxxopts::value<StringList>())
  ("o,outFile", "Output file name",cxxopts::value<string>())
  ("v,verbose", "Output formulas",cxxopts::value<bool>()->default_value("false"))
  ("h,help", "Print usage");
  // clang-format on

  auto result = options.parse(argc, argv);

  bool verbose = result["verbose"].as<bool>();
  string outFile;
  StringList nnfList;
  StringList cnfList;

  if (result.count("outFile")) { outFile = result["outFile"].as<string>(); }
  if (result.count("nnfFile")) { nnfList = result["nnfFile"].as<StringList>(); }
  if (result.count("cnfFile")) { cnfList = result["cnfFile"].as<StringList>(); }

  if (result.count("help") || (nnfList.empty() && cnfList.empty())) {
    std::cout << options.help() << std::endl;
    return 0;
  }

  std::ofstream ofs(outFile, std::ostream::out);
  if (!ofs.is_open() && !outFile.empty()) {
    std::cerr << "Unable to open output file!\n";
    throw new invalid_argument("Could not open file");
  }

  vasSAT::Parser p;
  vasSAT::Solver s;

  for (string &str : cnfList) {
    auto cnf = p.parseCNFFile(str);
    bool sat = s.Solve(cnf);

    if (verbose) {
      std::cout << str << "INTERNAL CNF FORMULA \n";
      cnf->print(std::cout);
      std::cout << "\n\n";
    }

    if (ofs.is_open()) { ofs << str << " RESULTS:"; }
    std::cout << str << " RESULTS:";
    if (sat) {
      if (ofs.is_open()) {
        ofs << "SAT\n";
        if (verbose) {
          cnf->printAssignment(ofs);
          ofs << "\n\n";
        }
      }
      cout << "SAT\n";
      if (verbose) {
        cnf->printAssignment(std::cout);
        cout << "\n\n";
      }
    } else {
      if (ofs.is_open()) { ofs << "UNSAT\n"; }
      cout << "UNSAT\n";
    }
  }

  for (string &str : nnfList) {
    auto nnf = p.parseNNfFile(str);
    vasSAT::CNFRef cnf = make_unique<vasSAT::CNFFormula>();
    nnf->buildCNF(cnf);

    if (verbose) {
      std::cout << "INTERMEDIATE REPRESENTATIONS FOR NNF FORMULA:" << str
                << "\n\n";
      std::cout << "PARSED NNF FORMULA:\n";
      nnf->print(std::cout);
      std::cout << "\n\n"
                << "EXTERNAL ID TO INTERNAL ID MAPPING:\n";
      nnf->printExternalToInternal(std::cout);
      std::cout << "\n\n"
                << "GENERATED CNF FORMULA\n";
      cnf->print(std::cout);
      std::cout << "\n\n";
      std::cout.flush();
    }

    bool sat = s.Solve(cnf);

    if (ofs.is_open()) { ofs << str << " RESULTS:"; }
    std::cout << str << " RESULTS:";
    if (sat) {
      if (ofs.is_open()) {
        ofs << "SAT\n";
        if (verbose) {
          cnf->printAssignment(ofs);
          ofs << "\n\n";
        }
      }
      cout << "SAT\n";
      if (verbose) {
        cnf->printAssignment(std::cout);
        cout << "\n\n";
      }
    } else {
      if (ofs.is_open()) { ofs << "UNSAT\n"; }
      cout << "UNSAT\n";
    }
  }
  if (ofs.is_open()) ofs.close();
  return 0;
}