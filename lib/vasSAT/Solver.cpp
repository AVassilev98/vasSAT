#include "vasSAT/Solver.hpp"
#include <stack>

namespace vasSAT {
struct Solver::DecisionData {
  unsigned var;
  std::stack<unsigned> props;
};

void Solver::initVtcMap(VarToClauseMap &vtcMap,
                        const std::unique_ptr<CNFFormula> &F) const {
  for (auto &clause : F->m_clauses) {
    for (unsigned lit : clause) {
      if (vtcMap.find(lit / 2) == vtcMap.end()) {
        vtcMap.insert({lit / 2, std::vector<Clause *>()});
      }
      vtcMap.find(lit / 2)->second.push_back(&clause);
    }
  }
}

bool Solver::Solve(std::unique_ptr<CNFFormula> &F) const {

  VarToClauseMap vtcMap;
  CNFFormula::AssignmentMap asgnMap;

  initVtcMap(vtcMap, F);

  return DPLL(vtcMap, F);
}

bool Solver::DPLL(VarToClauseMap &vtcMap,
                  std::unique_ptr<CNFFormula> &F) const {
  bool conflict = false;

  // each decision has a list of consequenting clause propagations and a node
  // in the decision tree
  std::stack<DecisionData> decisions;

  auto unitProp = [&](DecisionData &dd) {
    std::stack<unsigned> toProp;
    toProp.push(dd.var);

    while (!toProp.empty()) {
      unsigned var = toProp.top();
      toProp.pop();

      // go to every clause the variable is in
      for (auto &clause : vtcMap.find(var)->second) {
        bool satisfied = false;
        unsigned numEmpty = 0;
        unsigned emptyIdx = 0;

        for (int i = 0; i < clause->size(); i++) {
          unsigned lit = (*clause)[i];
          bool negation = lit % 2;
          auto asgn = F->m_asgnMap[lit / 2];

          if (asgn == CNFFormula::Assignment::Empty) {
            numEmpty++;
            emptyIdx = i;

            // if there is more than one unassigned variable in the clause we
            // can't infer anything
            if (numEmpty > 1) { break; }
          }
          // we can skip the clause if there is a single "true"
          else if ((negation && asgn == CNFFormula::Assignment::False) ||
                   (!negation && asgn == CNFFormula::Assignment::True)) {
            satisfied = true;
            break;
          }
        }
        if (satisfied || numEmpty > 1) continue;
        if (numEmpty == 0) {
          conflict = true;
          return;
        }
        unsigned var = (*clause)[emptyIdx] / 2;
        if ((*clause)[emptyIdx] % 2) {
          F->m_asgnMap[var] = CNFFormula::Assignment::False;
        } else {
          F->m_asgnMap[var] = CNFFormula::Assignment::True;
        }
        dd.props.push(var);
        toProp.push(var);
      }
    }
  };

  for (unsigned i = 0; i < F->m_asgnMap.size();) {

    if (!conflict) {
      auto &asgn = F->m_asgnMap[i];

      if (asgn == CNFFormula::Assignment::Empty) {
        F->m_asgnMap[i] = CNFFormula::Assignment::False;
        decisions.push({i, std::stack<unsigned>()});
        unitProp(decisions.top());
      }
      ++i;
    } else {

      auto &data = decisions.top();
      auto idx = data.var;
      while (!data.props.empty()) {
        F->m_asgnMap[data.props.top()] = CNFFormula::Assignment::Empty;
        data.props.pop();
      }

      if (F->m_asgnMap[idx] == CNFFormula::Assignment::False) {
        F->m_asgnMap[idx] = CNFFormula::Assignment::True;
        i = data.var;
        conflict = false;
        unitProp(decisions.top());
      } else {
        // top-level conflict
        if (decisions.size() == 1) return false;

        F->m_asgnMap[idx] = CNFFormula::Assignment::Empty;
        decisions.pop();
        i = decisions.top().var;
      }
    }
  }
  return true;
}

} // namespace vasSAT
