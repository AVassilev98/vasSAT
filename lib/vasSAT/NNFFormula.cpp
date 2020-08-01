#include <assert.h>
#include <stack>
#include <unordered_set>

#include "vasSAT/CNFFormula.hpp"
#include "vasSAT/NNFFormula.hpp"
#include "vasSAT/Node.hpp"

namespace {
using namespace vasSAT;

class PrettyPrinter : public AbstractNodeDispatcher {
  std::ostream &m_os;

public:
  PrettyPrinter(std::ostream &os) : m_os(os){};
  void Dispatch(AndNode &N) override {
    m_os << "(";
    N.getLeft().value()->Accept(*this);
    m_os << ".";
    N.getRight().value()->Accept(*this);
    m_os << ")";
  }
  void Dispatch(OrNode &N) override {
    m_os << "(";
    N.getLeft().value()->Accept(*this);
    m_os << "+";
    N.getRight().value()->Accept(*this);
    m_os << ")";
  }
  void Dispatch(NotNode &N) override {
    m_os << "(";
    m_os << "-";
    N.getRight().value()->Accept(*this);
    m_os << ")";
  }
  void Dispatch(LitNode &N) override { m_os << N.getExternalID(); }
};

class HeightDispatcher : public AbstractNodeDispatcher {
private:
  std::unordered_map<unsigned, std::vector<NodeRef>> &m_heightMap;

public:
  HeightDispatcher(
      std::unordered_map<unsigned, std::vector<NodeRef>> &height_map)
      : m_heightMap(height_map) {}

  void Dispatch(AndNode &N) override {
    auto left = N.getLeft().value();
    auto right = N.getRight().value();

    left->Accept(*this);
    right->Accept(*this);

    unsigned height = std::max(left->getHeight(), right->getHeight()) + 1;

    N.setHeight(height);
    if (m_heightMap.find(left->getHeight()) == m_heightMap.end()) {
      m_heightMap.insert({left->getHeight(), std::vector<NodeRef>()});
    }
    if (m_heightMap.find(right->getHeight()) == m_heightMap.end()) {
      m_heightMap.insert({right->getHeight(), std::vector<NodeRef>()});
    }
    m_heightMap.find(left->getHeight())->second.push_back(left);
    m_heightMap.find(right->getHeight())->second.push_back(right);
  }
  void Dispatch(OrNode &N) override {
    auto left = N.getLeft().value();
    auto right = N.getRight().value();

    left->Accept(*this);
    right->Accept(*this);

    unsigned height = std::max(left->getHeight(), right->getHeight()) + 1;

    N.setHeight(height);
    if (m_heightMap.find(left->getHeight()) == m_heightMap.end()) {
      m_heightMap.insert({left->getHeight(), std::vector<NodeRef>()});
    }
    if (m_heightMap.find(right->getHeight()) == m_heightMap.end()) {
      m_heightMap.insert({right->getHeight(), std::vector<NodeRef>()});
    }
    m_heightMap.find(left->getHeight())->second.push_back(left);
    m_heightMap.find(right->getHeight())->second.push_back(right);
  }
  void Dispatch(NotNode &N) override {
    auto right = N.getRight().value();

    right->Accept(*this);

    unsigned height = right->getHeight() + 1;

    N.setHeight(height);
    if (m_heightMap.find(right->getHeight()) == m_heightMap.end()) {
      m_heightMap.insert({right->getHeight(), std::vector<NodeRef>()});
    }
    m_heightMap.find(right->getHeight())->second.push_back(right);
  }
  void Dispatch(LitNode &N) override { N.setHeight(0); }
};

class CNFDispatcher : public AbstractNodeDispatcher {
private:
  CNFFormula &m_formula;

  // because the nodes are merged it is possible to visit the same NodeData more
  // than once, in which case we don't want to add duplicate clauses to our CNF
  std::unordered_set<NodeDataRef> m_visited;

public:
  CNFDispatcher(CNFFormula &F) : m_formula(F) {}

  void Dispatch(AndNode &N) override {
    if (m_visited.find(N.getData()) != m_visited.end()) return;
    m_visited.insert(N.getData());

    int curID = (int)N.getID();
    int leftID = (int)N.getLeft().value()->getID();
    int rightID = (int)N.getLeft().value()->getID();

    m_formula.addClause({curID, -leftID, -rightID});
    m_formula.addClause({-curID, leftID});
    m_formula.addClause({-curID, rightID});

    N.getLeft().value()->Accept(*this);
    N.getRight().value()->Accept(*this);
  }
  void Dispatch(OrNode &N) override {
    if (m_visited.find(N.getData()) != m_visited.end()) return;
    m_visited.insert(N.getData());

    int curID = (int)N.getID();
    int leftID = (int)N.getLeft().value()->getID();
    int rightID = (int)N.getLeft().value()->getID();

    m_formula.addClause({-curID, leftID, rightID});
    m_formula.addClause({curID, -leftID});
    m_formula.addClause({curID, -rightID});

    N.getLeft().value()->Accept(*this);
    N.getRight().value()->Accept(*this);
  }
  void Dispatch(NotNode &N) override {
    if (m_visited.find(N.getData()) != m_visited.end()) return;
    m_visited.insert(N.getData());

    int curID = (int)N.getID();
    int rightID = (int)N.getLeft().value()->getID();

    m_formula.addClause({-curID, -rightID});
    m_formula.addClause({curID, rightID});

    N.getRight().value()->Accept(*this);
  }
  void Dispatch(LitNode &N) override { return; }
};

class ValidityDispatcher : public AbstractNodeDispatcher {
private:
  bool m_isValid = true;

public:
  ValidityDispatcher() = default;
  bool isValid() const { return m_isValid; }

  void Dispatch(AndNode &N) override {
    if (!m_isValid) return;

    if (!N.getLeft().has_value() || !N.getLeft().has_value()) {
      m_isValid = false;
      return;
    }

    N.getLeft().value()->Accept(*this);
    N.getRight().value()->Accept(*this);
  }
  void Dispatch(OrNode &N) override {
    if (!m_isValid) return;

    if (!N.getLeft().has_value() || !N.getLeft().has_value()) {
      m_isValid = false;
      return;
    }

    N.getLeft().value()->Accept(*this);
    N.getRight().value()->Accept(*this);
  }
  void Dispatch(NotNode &N) override {
    if (!m_isValid) return;

    if (!N.getRight().has_value()) m_isValid = false;
    else if (N.getRight().value()->getType() != NodeType::LIT)
      m_isValid = false;
    return;
  }
  void Dispatch(LitNode &N) override { return; }
};
} // namespace

namespace vasSAT {

bool NNFFormula::isValid() const {
  ValidityDispatcher vd;
  m_rootNode->Accept(vd);
  return vd.isValid();
}

void NNFFormula::populateHeightMap() {
  HeightDispatcher hd(m_heightMap);
  m_rootNode->Accept(hd);

  switch (m_rootNode->getType()) {
  case NodeType::AND:
  case NodeType::OR:
    m_rootNode->setHeight(
        1 + std::max(m_rootNode->getLeft().value()->getHeight(),
                     m_rootNode->getRight().value()->getHeight()));

    break;
  case NodeType::NOT:
    m_rootNode->setHeight(1 + m_rootNode->getRight().value()->getHeight());
    break;
  case NodeType::LIT:
    m_rootNode->setHeight(0);
    break;
  default:
    break;
  }
}

void NNFFormula::printExternalToInternal(std::ostream &os) const {
  std::stack<NodeRef> toVisit;

  toVisit.push(m_rootNode);
  while (!toVisit.empty()) {
    auto &ref = toVisit.top();

    if (ref->getType() == NodeType::LIT) {
      const auto &litNode = std::dynamic_pointer_cast<LitNode>(ref);
      int externalID = litNode->getExternalID();
      int internalID = litNode->getID();

      os << externalID << " ---> " << internalID << "\n";
    }

    if (ref->getLeft().has_value()) { toVisit.push(ref->getLeft().value()); }
    if (ref->getRight().has_value()) { toVisit.push(ref->getRight().value()); }
  }
  os.flush();
}

void NNFFormula::print(std::ostream &os) const {
  PrettyPrinter pp(os);
  m_rootNode->Accept(pp);
  os << " 0" << std::endl;
}

void NNFFormula::mergeNodes() {}

} // namespace vasSAT
