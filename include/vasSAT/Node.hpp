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

enum class NodeType { AND, OR, NOT, LIT };

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
  std::optional<NodeRef> nodeLeft;
  std::optional<NodeRef> nodeRight;
};

class Node {
protected:
  NodeDataRef m_data;

public:
  Node(int id) : m_data(std::make_shared<NodeData>()) { m_data->id = id; }
  inline unsigned getHeight() const { return m_data->height; }
  inline unsigned getID() const { return m_data->id; }
  inline std::optional<NodeRef> const getLeft() { return m_data->nodeLeft; }
  inline std::optional<NodeRef> const getRight() { return m_data->nodeRight; }

  inline NodeDataRef getData() { return m_data; }

  inline virtual NodeType getType() const = 0;
  virtual void insertLeft(const NodeRef N) = 0;
  virtual void insertRight(const NodeRef N) = 0;
  void setHeight(unsigned h) { m_data->height = h; }

  virtual void Accept(AbstractNodeDispatcher &dispatcher) = 0;

  void merge(const NodeRef &N) { m_data = N->getData(); }

  virtual bool isIsomorphic(const NodeRef &N) {
    if (getType() != N->getType()) return false;
    auto left = m_data->nodeLeft.value()->getData();
    auto leftOther = N->getLeft().value()->getData();

    auto right = m_data->nodeRight.value()->getData();
    auto rightOther = N->getRight().value()->getData();

    return (left == leftOther && right == rightOther) ||
           (left == rightOther && right == leftOther);
  }

  virtual ~Node() = default;
};

class AndNode : public Node {
public:
  AndNode(int id) : Node(id) {}

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
  OrNode(int id) : Node(id) {}

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
  NotNode(int id) : Node(id) {}

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

  virtual bool isIsomorphic(const NodeRef &N) override {
    if (getType() != N->getType()) return false;
    auto right = m_data->nodeRight.value()->getData();
    auto rightOther = N->getRight().value()->getData();

    return (right == rightOther);
  }
};

class LitNode : public Node {
private:
  int m_externalID = 0;

public:
  LitNode(int id, int externalID) : Node(id), m_externalID(externalID) {}

  int getExternalID() const { return m_externalID; }

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

  bool isIsomorphic(const NodeRef &N) override {
    if (N->getType() != NodeType::LIT)
      assert(0 && "Can't compare lit to non-lit!\n");

    return m_externalID ==
           std::dynamic_pointer_cast<LitNode>(N)->getExternalID();
  }
};
} // namespace vasSAT