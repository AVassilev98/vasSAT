#include "vasSAT/Solver.hpp"
#include <stack>

namespace vasSAT {
struct Solver::DecisionData {
  unsigned var;
  CNFFormula::AssignmentMap::iterator it;
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

  std::stack<unsigned> vars;
  for (auto &pair : F->m_asgnMap) {
    vars.push(pair.first);
  }

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
          auto asgn = F->m_asgnMap.find(lit / 2)->second;

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
          F->m_asgnMap.find(var)->second = CNFFormula::Assignment::False;
        } else {
          F->m_asgnMap.find(var)->second = CNFFormula::Assignment::True;
        }
        dd.props.push(var);
        toProp.push(var);
      }
    }
  };

  for (auto it = F->m_asgnMap.begin(); it != F->m_asgnMap.end();) {

    if (!conflict) {
      auto &pair = *it;

      if (pair.second == CNFFormula::Assignment::Empty) {
        pair.second = CNFFormula::Assignment::False;
        decisions.push({pair.first, it, std::stack<unsigned>()});
        unitProp(decisions.top());
      }
      ++it;
    } else {

      auto &data = decisions.top();
      auto &pair = *data.it;
      while (!data.props.empty()) {
        F->m_asgnMap.find(data.props.top())->second =
            CNFFormula::Assignment::Empty;
        data.props.pop();
      }

      if (pair.second == CNFFormula::Assignment::False) {
        pair.second = CNFFormula::Assignment::True;
        it = data.it;
        conflict = false;
        unitProp(decisions.top());
      } else {
        // top-level conflict
        if (decisions.size() == 1) return false;

        decisions.pop();
        it = decisions.top().it;
      }
    }
  }
  return true;
}

} // namespace vasSAT
