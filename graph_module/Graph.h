#ifndef GRAPH_H
#define GRAPH_H

#include "Array.h"
#include "Utils.h"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <new>
#include <optional>
#include <sstream>
#include <string_view>
#include <type_traits>

template <typename T>
concept GraphData = !std::is_pointer_v<T> && std::copyable<T> && std::movable<T> &&
                    std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T> &&
                    std::default_initializable<T> && requires(std::ostream& os, std::istream& is, T data) {
                      { os << data } -> std::same_as<std::ostream&>;
                      { is >> data } -> std::same_as<std::istream&>;
                    };

template <GraphData NData, GraphData EData> class Graph {
public:
  class Nodes;

  class Edges;

  virtual ~Graph() = default;
  Graph(const Graph& other);
  Graph(Graph&& other) noexcept;

  const Nodes& nodes() const;
  const Edges& edges() const;
  Nodes& nodes();
  Edges& edges();
  void print(std::ostream& stream = std::cout) const;
  void print(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open())
      throw FileException(UNABLE_TO_OPEN_OUTPUT_FILE, filename);
    print(file);
  }
  void import(std::istream& stream = std::cin);
  void import(std::string_view filename);

  Graph& operator=(const Graph& other);
  Graph& operator=(Graph&& other) noexcept;

  friend std::ostream& operator<<(std::ostream& stream, const Graph& g) {
    g.print(stream);
    return stream;
  }

  void clear();

private:
  using AdjMatrix = lib::Array<lib::Array<std::optional<Identifier>>>;
  AdjMatrix adjMatrix_;
  Nodes nodes_;
  Edges edges_;

  friend class Nodes;
  friend class Edges;

  void expandMatrix();

  void parseNode(std::istringstream& ss);
  void parseEdge(std::istringstream& ss);

  virtual bool isDirected() const = 0;

protected:
  Graph() : nodes_(this), edges_(this) {}
};

template <GraphData NData, GraphData EData>
Graph<NData, EData>& Graph<NData, EData>::operator=(const Graph& other) {
  if (this == &other)
    return *this;
  try {
    this->adjMatrix_ = other.adjMatrix_;
    this->nodes_ = other.nodes_;
    this->edges_ = other.edges_;
    nodes_.owner_ = this;
    edges_.owner_ = this;
  } catch (const std::bad_alloc&) {
    throw MemoryException(UNAVAILABLE_MEMORY_ASSIGN);
  }

  return *this;
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>& Graph<NData, EData>::operator=(Graph&& other) noexcept {
  if (this == &other)
    return *this;
  clear();
  adjMatrix_ = std::move(other.adjMatrix_);
  nodes_ = std::move(other.nodes_);
  edges_ = std::move(other.edges_);
  nodes_.owner_ = this;
  edges_.owner_ = this;
  other.nodes_.owner_ = &other;
  other.edges_.owner_ = &other;
  return *this;
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::clear() {
  adjMatrix_.clear();
  nodes_.clear();
  edges_.clear();
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Graph(const Graph& other) try
    : adjMatrix_(other.adjMatrix_), nodes_(other.nodes_), edges_(other.edges_) {
  nodes_.owner_ = this;
  edges_.owner_ = this;
} catch (const std::bad_alloc&) {
  throw MemoryException(UNAVAILABLE_MEMORY_COPY);
} catch (const MemoryException&) {
  throw MemoryException(UNAVAILABLE_MEMORY_COPY);
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Graph(Graph&& other) noexcept
    : adjMatrix_(std::move(other.adjMatrix_)), nodes_(std::move(other.nodes_)),
      edges_(std::move(other.edges_)) {
  nodes_.owner_ = this;
  edges_.owner_ = this;
  other.nodes_.owner_ = &other;
  other.edges_.owner_ = &other;
}
template <GraphData NData, GraphData EData> void Graph<NData, EData>::parseNode(std::istringstream& ss) {
  char c;
  Identifier id;
  NData data;
  ss >> c >> id >> c;
  std::string rawData;
  std::getline(ss, rawData, '}');
  std::istringstream(rawData) >> data;
  nodes_.add(id, std::move(data));
}

template <GraphData NData, GraphData EData>
void Graph<NData, EData>::parseEdge(std::istringstream& inputStream) {
  char c;
  Identifier source, id, target;
  EData data;
  inputStream >> c >> source >> c >> c;
  inputStream >> c >> id;
  inputStream >> c;
  std::string rawData;
  std::getline(inputStream, rawData, '}');
  std::istringstream(rawData) >> data;
  inputStream >> c >> c >> c >> c >> target >> c;
  edges_.add(id, source, target, std::move(data));
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::import(std::istream& inputStream) {
  try {
    std::string line;
    while (std::getline(inputStream, line)) {
      if (line.empty())
        continue;
      std::istringstream lineStream(line);
      std::string keyword;
      lineStream >> keyword;
      if (keyword == "node")
        parseNode(lineStream);
      else if (keyword == "edge")
        parseEdge(lineStream);
    }
  } catch (const std::bad_alloc&) {
    throw MemoryException(UNAVAILABLE_MEMORY_IMPORT);
  } catch (const MemoryException&) {
    throw MemoryException(UNAVAILABLE_MEMORY_IMPORT);
  }
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::import(std::string_view filename) {
  try {
    std::ifstream file((std::string(filename)));
    if (!file.is_open())
      throw FileException(UNABLE_TO_OPEN_INPUT_FILE, filename);
    import(file);
  } catch (const std::bad_alloc&) {
    throw MemoryException(UNAVAILABLE_MEMORY_IMPORT);
  } catch (const MemoryException&) {
    throw MemoryException(UNAVAILABLE_MEMORY_IMPORT);
  }
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::print(std::ostream& stream) const {
  nodes().print(stream);
  edges().print(stream);
}

template <GraphData NData, GraphData EData> class DirectedGraph : public Graph<NData, EData> {
public:
  DirectedGraph() : Graph<NData, EData>() {}

private:
  bool isDirected() const final override { return true; };
};

template <GraphData NData, GraphData EData> class UndirectedGraph : public Graph<NData, EData> {
public:
  UndirectedGraph() : Graph<NData, EData>() {}

private:
  bool isDirected() const override final { return false; };
};

template <GraphData NData, GraphData EData>
inline const Graph<NData, EData>::Nodes& Graph<NData, EData>::nodes() const {
  return nodes_;
}
template <GraphData NData, GraphData EData>
inline const Graph<NData, EData>::Edges& Graph<NData, EData>::edges() const {
  return edges_;
}
template <GraphData NData, GraphData EData> inline Graph<NData, EData>::Nodes& Graph<NData, EData>::nodes() {
  return nodes_;
}
template <GraphData NData, GraphData EData> inline Graph<NData, EData>::Edges& Graph<NData, EData>::edges() {
  return edges_;
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::expandMatrix() {
  size_t n = adjMatrix_.size();
  size_t columns_added = 0;

  try {
    for (size_t i = 0; i < n; ++i) {
      adjMatrix_[i].push_back(std::nullopt);
      ++columns_added;
    }
    lib::Array<std::optional<Identifier>> new_row;
    for (size_t j = 0; j <= n; ++j)
      new_row.push_back(std::nullopt);
    adjMatrix_.push_back(std::move(new_row));
  } catch (...) {
    for (size_t i = 0; i < columns_added; ++i) {
      adjMatrix_[i].pop_back();
    }
    throw;
  }
}

template <GraphData NData, GraphData EData> class Graph<NData, EData>::Nodes {
public:
  friend class Graph;
  class Node;

  explicit Nodes(Graph* owner) : owner_(owner) {}

  bool exists(Identifier id) const;
  size_t size() const;
  Node& add(const NData& data);
  Node& add(NData&& data);
  Node& add(Identifier id, const NData& data);
  Node& add(Identifier id, NData&& data);

  Node& get(Identifier id);
  const Node& get(Identifier id) const;
  Node& operator[](Identifier id);
  const Node& operator[](Identifier id) const;

  auto begin() { return nodes_.begin(); }
  auto end() { return nodes_.end(); }
  auto begin() const { return nodes_.cbegin(); }
  auto end() const { return nodes_.cend(); }
  auto cbegin() const { return nodes_.cbegin(); }
  auto cend() const { return nodes_.cend(); }

  void print(std::ostream& stream = std::cout) const;

  void clear();

  friend std::ostream& operator<<(std::ostream& stream, const Nodes& nodes) {
    for (const auto& node : nodes.nodes_)
      stream << node << '\n';
    return stream;
  }

private:
  Graph* owner_;
  lib::Array<Node> nodes_;

  void validateNewNode(Identifier id);
  template <typename T> Node& addInternal(Identifier id, T&& data);
};
template <GraphData NData, GraphData EData> void Graph<NData, EData>::Nodes::clear() { nodes_.clear(); }

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::get(Identifier id) {
  if (!exists(id))
    throw ElementException(NODE_DOES_NOT_EXIST, id);
  return nodes_.at(id);
}

template <GraphData NData, GraphData EData>
const Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::get(Identifier id) const {
  if (!exists(id))
    throw ElementException(NODE_DOES_NOT_EXIST, id);
  return nodes_.at(id);
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::operator[](Identifier id) {
  return get(id);
}

template <GraphData NData, GraphData EData>
const Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::operator[](Identifier id) const {
  return get(id);
}

template <GraphData NData, GraphData EData>
void Graph<NData, EData>::Nodes::print(std::ostream& stream) const {
  stream << *(this);
}

template <GraphData NData, GraphData EData> bool Graph<NData, EData>::Nodes::exists(Identifier id) const {
  return size() > id;
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::Nodes::validateNewNode(Identifier id) {
  if (id == SIZE_MAX) {
    throw IdentifierException(INVALID_NODE_IDENTIFIER, id);
  }
  if (exists(id)) {
    throw ConflictException(NODE_ALREADY_EXISTS, id);
  }
  if (id != nodes_.size()) {
    throw IdentifierException(NON_SUCCESSIVE_NODE, id);
  }
}

template <GraphData NData, GraphData EData>
template <typename T>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::addInternal(Identifier id, T&& data) {
  validateNewNode(id);
  try {
    nodes_.emplace_back(id, std::forward<T>(data));
    owner_->expandMatrix();
  } catch (const std::bad_alloc&) {
    if (nodes_.size() > id)
      nodes_.pop_back();
    throw MemoryException(UNAVAILABLE_MEMORY_NODE);
  } catch (...) {
    if (nodes_.size() > id)
      nodes_.pop_back();
    throw;
  }
  return nodes_.at(id);
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::add(const NData& data) {
  return addInternal(nodes_.size(), data);
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::add(Identifier id, const NData& data) {
  return addInternal(id, data);
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::add(NData&& data) {
  return addInternal(nodes_.size(), std::move(data));
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Nodes::Node& Graph<NData, EData>::Nodes::add(Identifier id, NData&& data) {
  return addInternal(id, std::move(data));
}

template <GraphData NData, GraphData EData> size_t Graph<NData, EData>::Nodes::size() const {
  return nodes_.size();
}

template <GraphData NData, GraphData EData> class Graph<NData, EData>::Edges {
public:
  friend class Graph;
  class Edge;
  struct EdgeProxy {
    Edges& edges_;
    Identifier source_;
    Edge& operator[](Identifier target) { return edges_.get(source_, target); }
  };
  struct ConstEdgeProxy {
    const Edges& edges_;
    Identifier source_;
    const Edge& operator[](Identifier target) const { return edges_.get(source_, target); }
  };

  EdgeProxy operator[](Identifier source) { return EdgeProxy{*this, source}; }
  ConstEdgeProxy operator[](Identifier source) const { return ConstEdgeProxy{*this, source}; }

  explicit Edges(Graph* owner) : owner_(owner) {}

  size_t size() const;

  bool exists(Identifier id) const;
  bool exists(Identifier source, Identifier target) const;

  Edge& get(Identifier id);
  const Edge& get(Identifier id) const;
  Edge& get(Identifier source, Identifier target);
  const Edge& get(Identifier source, Identifier target) const;

  auto begin() { return edges_.begin(); }
  auto end() { return edges_.end(); }
  auto begin() const { return edges_.cbegin(); }
  auto end() const { return edges_.cend(); }
  auto cbegin() const { return edges_.cbegin(); }
  auto cend() const { return edges_.cend(); }

  void print(std::ostream& stream = std::cout) const;
  void printMatrix(std::ostream& stream = std::cout) const;

  friend std::ostream& operator<<(std::ostream& stream, const Edges& edges) {
    for (size_t i = 0; i < edges.size(); ++i) {
      stream << edges.edges_[i] << std::endl;
    }
    return stream;
  }

  Edge& add(Identifier id, Identifier source, Identifier target, const EData& data);
  Edge& add(Identifier id, Identifier source, Identifier target, EData&& data);
  Edge& add(Identifier source, Identifier target, const EData& data);
  Edge& add(Identifier source, Identifier target, EData&& data);

  void clear();

private:
  template <typename T> Edge& addInternal(Identifier id, Identifier source, Identifier target, T&& data);
  void validateEdge(Identifier id, Identifier source, Identifier target);

  Graph* owner_;
  lib::Array<Edge> edges_;
};

template <GraphData NData, GraphData EData> size_t Graph<NData, EData>::Edges::size() const {
  return edges_.size();
}

template <GraphData NData, GraphData EData> void Graph<NData, EData>::Edges::clear() { edges_.clear(); };

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::get(Identifier id) {
  if (!exists(id))
    throw ElementException(EDGE_DOES_NOT_EXIST, id);
  return edges_.at(id);
}

template <GraphData NData, GraphData EData>
const Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::get(Identifier id) const {
  if (!exists(id))
    throw ElementException(EDGE_DOES_NOT_EXIST, id);
  return edges_.at(id);
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::get(Identifier source, Identifier target) {
  if (!owner_->nodes_.exists(source))
    throw ElementException(SOURCE_NOT_EXIST, source);
  if (!owner_->nodes_.exists(target))
    throw ElementException(TARGET_NOT_EXIST, target);
  if (!exists(source, target))
    throw ElementException(EDGE_BETWEEN_SRC_TRGT_NOT_EXIST, source, target);
  return edges_.at(owner_->adjMatrix_[source][target].value());
}

template <GraphData NData, GraphData EData>
const Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::get(Identifier source,
                                                                        Identifier target) const {
  if (!owner_->nodes_.exists(source))
    throw ElementException(SOURCE_NOT_EXIST, source);
  if (!owner_->nodes_.exists(target))
    throw ElementException(TARGET_NOT_EXIST, target);
  if (!exists(source, target))
    throw ElementException(EDGE_BETWEEN_SRC_TRGT_NOT_EXIST, source, target);
  return edges_.at(owner_->adjMatrix_[source][target].value());
}

template <GraphData NData, GraphData EData>
void Graph<NData, EData>::Edges::print(std::ostream& stream) const {
  stream << *(this);
}
template <GraphData NData, GraphData EData>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::add(Identifier id, Identifier source,
                                                                  Identifier target, const EData& data) {
  return addInternal(id, source, target, data);
}
template <GraphData NData, GraphData EData>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::add(Identifier id, Identifier source,
                                                                  Identifier target, EData&& data) {
  return addInternal(id, source, target, std::move(data));
}
template <GraphData NData, GraphData EData>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::add(Identifier source, Identifier target,
                                                                  const EData& data) {
  Identifier id = size();
  return addInternal(id, source, target, std::move(data));
}

template <GraphData NData, GraphData EData>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::add(Identifier source, Identifier target,
                                                                  EData&& data) {
  Identifier id = size();
  return addInternal(id, source, target, std::move(data));
}

template <GraphData NData, GraphData EData>
template <typename T>
Graph<NData, EData>::Edges::Edge& Graph<NData, EData>::Edges::addInternal(Identifier id, Identifier source,
                                                                          Identifier target, T&& data) {
  validateEdge(id, source, target);
  try {
    edges_.emplace_back(id, source, target, std::forward<T>(data));
  } catch (const std::bad_alloc&) {
    throw MemoryException(UNAVAILABLE_MEMORY_EDGE);
  }
  owner_->adjMatrix_[source][target] = id;
  if (!owner_->isDirected())
    owner_->adjMatrix_[target][source] = id;
  return edges_.at(id);
}

template <GraphData NData, GraphData EData> bool Graph<NData, EData>::Edges::exists(Identifier id) const {
  return (size() > id);
}

template <GraphData NData, GraphData EData>
bool Graph<NData, EData>::Edges::exists(Identifier source, Identifier target) const {
  if (!owner_->nodes_.exists(source))
    throw ElementException(SOURCE_NOT_EXIST, source);
  if (!owner_->nodes_.exists(target))
    throw ElementException(TARGET_NOT_EXIST, target);
  return (owner_->adjMatrix_[source][target] != std::nullopt);
}

template <GraphData NData, GraphData EData>
void Graph<NData, EData>::Edges::validateEdge(Identifier id, Identifier source, Identifier target) {
  if (id == SIZE_MAX) {
    throw IdentifierException(INVALID_EDGE_IDENTIFIER, id);
  }
  if (exists(id)) {
    throw ConflictException(ID_EDGE_ALREADY_EXISTS, id);
  }
  if (id != edges_.size()) {
    throw IdentifierException(NON_SUCCESSIVE_EDGE, id);
  }
  if (!owner_->nodes().exists(source)) {
    throw ElementException(SOURCE_NOT_EXIST, source);
  }
  if (!owner_->nodes().exists(target)) {
    throw ElementException(TARGET_NOT_EXIST, target);
  }
  if (exists(source, target)) {
    throw ConflictException(EDGE_ALREADY_EXISTS, source, target);
  }
}

template <GraphData NData, GraphData EData>
void Graph<NData, EData>::Edges::printMatrix(std::ostream& stream) const {
  for (size_t row = 0; row < owner_->adjMatrix_.size(); ++row) {
    for (size_t column = 0; column + 1 < owner_->adjMatrix_[row].size(); ++column) {
      std::optional<Identifier> cell = owner_->adjMatrix_[row][column];
      if (cell)
        stream << cell.value() << '|';
      else
        stream << '-' << '|';
    }
    size_t last_column = owner_->adjMatrix_[row].size() - 1;
    std::optional<Identifier> cell = owner_->adjMatrix_[row][last_column];
    if (cell)
      stream << cell.value();
    else
      stream << '-';
    stream << std::endl;
  }
}

template <GraphData NData, GraphData EData> class Graph<NData, EData>::Nodes::Node {
public:
  Node(Identifier id, NData data) : id_(id), data_(std::move(data)) {}
  Identifier getId() const { return id_; }
  NData& getData() { return data_; }
  const NData& getData() const { return data_; }

  friend std::ostream& operator<<(std::ostream& stream, const Node& node) {
    stream << "node (" << node.id_ << " {" << node.data_ << "})";
    return stream;
  }

private:
  Identifier id_;
  NData data_;
};

template <GraphData NData, GraphData EData> class Graph<NData, EData>::Edges::Edge {
public:
  Edge(Identifier id, Identifier source, Identifier target, EData data)
      : id_(id), source_(source), target_(target), data_(std::move(data)) {}
  Identifier getId() const { return id_; }
  Identifier getSource() const { return source_; }
  Identifier getTarget() const { return target_; }
  EData& getData() { return data_; }
  const EData& getData() const { return data_; }

  friend std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
    stream << "edge (" << edge.getSource() << ")-[" << edge.getId() << " {" << edge.getData() << "}]->("
           << edge.getTarget() << ")";
    return stream;
  }

private:
  Identifier id_;
  Identifier source_;
  Identifier target_;
  EData data_;
};

#endif
