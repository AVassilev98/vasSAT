#include <unordered_set>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/DAG.hpp"

using namespace vasSAT;

class CNFDispatcher : public AbstractNodeDispatcher {
private:
  CNFFormula &m_formula;

  // because the nodes are merged it is possible to visit the same NodeData more
  // than once, in which case we don't want to add duplicate clauses to our CNF
  std::unordered_set<NodeDataRef> m_visited;

public:
  CNFDispatcher(CNFFormula &F) : m_formula(F) {}
  void Dispatch(AndNode &N) override;
  void Dispatch(OrNode &N) override;
  void Dispatch(NotNode &N) override;
  void Dispatch(LitNode &N) override;
};

void CNFDispatcher::Dispatch(AndNode &N) {
  if (m_visited.find(N.getData()) != m_visited.end()) return;
  m_visited.insert(N.getData());

  int curID = (int)N.getID();
  int leftID = (int)N.getLeft()->getID();
  int rightID = (int)N.getLeft()->getID();

  m_formula.addClause({curID, -leftID, -rightID});
  m_formula.addClause({-curID, leftID});
  m_formula.addClause({-curID, rightID});

  N.getLeft()->Accept(*this);
  N.getRight()->Accept(*this);
}
void CNFDispatcher::Dispatch(OrNode &N) {
  if (m_visited.find(N.getData()) != m_visited.end()) return;
  m_visited.insert(N.getData());

  int curID = (int)N.getID();
  int leftID = (int)N.getLeft()->getID();
  int rightID = (int)N.getLeft()->getID();

  m_formula.addClause({-curID, leftID, rightID});
  m_formula.addClause({curID, -leftID});
  m_formula.addClause({curID, -rightID});

  N.getLeft()->Accept(*this);
  N.getRight()->Accept(*this);
}
void CNFDispatcher::Dispatch(NotNode &N) {
  if (m_visited.find(N.getData()) != m_visited.end()) return;
  m_visited.insert(N.getData());

  int curID = (int)N.getID();
  int leftID = (int)N.getLeft()->getID();
  int rightID = (int)N.getLeft()->getID();

  m_formula.addClause({curID, -leftID, -rightID});
  m_formula.addClause({-curID, leftID});
  m_formula.addClause({-curID, rightID});

  N.getRight()->Accept(*this);
}
void CNFDispatcher::Dispatch(LitNode &N) { return; }