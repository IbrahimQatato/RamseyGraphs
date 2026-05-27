#include "ExhaustiveSearch.h"
#include <fstream>
#include <iostream>

ExhaustiveSearch::ExhaustiveSearch(size_t s, size_t t)
    : s_(s), t_(t), current_n_(std::max(s, t)), current_coloring_(0), done_(false), result_(0) {}

size_t ExhaustiveSearch::total_edges(size_t n) { return n * (n - 1) / 2; }

std::vector<std::pair<Identifier, Identifier>> ExhaustiveSearch::edge_order(size_t n) {
  std::vector<std::pair<Identifier, Identifier>> result;
  for (Identifier i = 0; i < n; ++i) {
    for (Identifier j = i + 1; j < n; ++j) {
      result.push_back({i, j});
    }
  }
  return result;
}

UndirectedGraph<Identifier, Color> ExhaustiveSearch::build_colored_kn(size_t n, uint64_t mask) const {
  UndirectedGraph<Identifier, Color> graph;
  for (size_t i = 0; i < n; ++i) {
    graph.nodes().add(i, i);
  }
  auto edges = edge_order(n);
  for (size_t i = 0; i < edges.size(); ++i) {
    Color color = ((mask >> i) & 1) ? Color::Blue : Color::Red;
    graph.edges().add(edges[i].first, edges[i].second, color);
  }
  return graph;
}

// wrapper to introduce caching
// to maybe implement a search function not limited to uint64_t sized numbers
UndirectedGraph<Identifier, Color> ExhaustiveSearch::getColoring(size_t n, uint64_t coloring_index) const {
  return build_colored_kn(n, coloring_index);
}

bool ExhaustiveSearch::isDone() const { return done_; }
size_t ExhaustiveSearch::result() const { return result_; }

const std::vector<uint64_t>& ExhaustiveSearch::counterexamples(size_t n) const {
  static const std::vector<uint64_t> empty;
  auto it = counterexamples_.find(n);
  return it != counterexamples_.end() ? it->second : empty;
}
