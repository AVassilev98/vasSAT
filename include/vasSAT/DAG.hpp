#pragma once

#include <assert.h>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace vasSAT {
class CNFFormula;

class AbstractNodeDispatcher;
class Node;
class NodeData;
using NodeRef = std::shared_ptr<Node>;
using NodeDataRef = std::shared_ptr<NodeData>;

class AndNode;
class OrNode;
class NotNode;
class LitNode;

class AbstractNodeDispatcher {
public:
  virtual void Dispatch(AndNode &N) = 0;
  virtual void Dispatch(OrNode &N) = 0;
  virtual void Dispatch(NotNode &N) = 0;
  virtual void Dispatch(LitNode &N) = 0;
};

struct NodeData {
  unsigned id;
  unsigned height;
  NodeRef nodeLeft;
  NodeRef nodeRight;
};

enum class NodeType { AND, OR, NOT, LIT };

class Node {
protected:
  static unsigned m_nodeID;

  NodeDataRef m_data;

public:
  Node(int id, int depth);
  inline unsigned getHeight() { return m_data->height; }
  inline unsigned getID() { return m_data->id; }
  inline NodeRef getLeft() { return m_data->nodeLeft; }
  inline NodeRef getRight() { return m_data->nodeRight; }

  inline NodeDataRef getData() { return m_data; };

  inline virtual NodeType getType() const = 0;
  virtual void insertLeft(const NodeRef N) = 0;
  virtual void insertRight(const NodeRef N) = 0;

  virtual void Accept(AbstractNodeDispatcher &dispatcher) = 0;
  void merge(const NodeRef &N) { m_data = N->getData(); }

  virtual ~Node();
};

class AndNode : public Node {
public:
  inline NodeType getType() const override { return NodeType::AND; }
  void insertLeft(const NodeRef N) override {
    m_data->nodeLeft = N;
    if (m_data->height <= N->getHeight()) m_data->height += N->getHeight() + 1;
  }
  void insertRight(const NodeRef N) override {
    m_data->nodeRight = N;
    if (m_data->height <= N->getHeight()) m_data->height += N->getHeight() + 1;
  }

  void Accept(AbstractNodeDispatcher &dispatcher) override {
    dispatcher.Dispatch(*this);
  }
};

class OrNode : public Node {
public:
  inline NodeType getType() const override { return NodeType::OR; }
  void insertLeft(const NodeRef N) override {
    m_data->nodeLeft = N;
    if (m_data->height <= N->getHeight()) m_data->height += N->getHeight() + 1;
  }
  void insertRight(const NodeRef N) override {
    m_data->nodeRight = N;
    if (m_data->height <= N->getHeight()) m_data->height += N->getHeight() + 1;
  }

  void Accept(AbstractNodeDispatcher &dispatcher) override {
    dispatcher.Dispatch(*this);
  }
};

class NotNode : public Node {
public:
  inline NodeType getType() const override { return NodeType::NOT; }
  void insertLeft(const NodeRef N) override {
    assert(0 && "Should not be inserting left on a NotNode!");
  }
  void insertRight(const NodeRef N) override {
    m_data->nodeRight = N;
    if (m_data->height <= N->getHeight()) m_data->height += N->getHeight() + 1;
  }

  void Accept(AbstractNodeDispatcher &dispatcher) override {
    dispatcher.Dispatch(*this);
  }
};

class LitNode : public Node {
public:
  inline NodeType getType() const override { return NodeType::LIT; }
  void insertLeft(const NodeRef N) override {
    assert(0 && "Should not be inserting left on a LitNode!");
  }
  void insertRight(const NodeRef N) override {
    assert(0 && "Should not be inserting right on a LitNode!");
  }

  void Accept(AbstractNodeDispatcher &dispatcher) override {
    dispatcher.Dispatch(*this);
  }
};

class DAG {
private:
  CNFFormula m_formula;
  NodeRef m_rootNode;
  std::vector<NodeRef> m_nodes;
  std::unordered_map<unsigned, NodeRef> m_heightMap;

  void buildCNF();
};

} // namespace vasSAT