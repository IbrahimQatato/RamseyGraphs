#include "ExhaustiveSearch.h"
#include "RamseyChecker.h"
#include "Utils.h"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <vector>

ExhaustiveSearch::ExhaustiveSearch(size_t s, size_t t)
    : SearchStrategy(s, t, std::max(s, t)), current_coloring_(0) {}


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

UndirectedGraph<Identifier, Color> ExhaustiveSearch::getColoring(size_t n, uint64_t coloring_index) const {
  return build_colored_kn(n, coloring_index);
}


const std::vector<uint64_t>& ExhaustiveSearch::counterexamples(size_t n) const {
  static const std::vector<uint64_t> empty;
  auto it = counterexamples_.find(n);
  return it != counterexamples_.end() ? it->second : empty;
}

void ExhaustiveSearch::test_n(size_t n, bool verbose) {
  size_t edges = total_edges(n);
  uint64_t total_colorings = 1ULL << edges;
  std::cout << "\nTesting K_" << n << " (" << edges << " edges, " << total_colorings << " colorings)...\n";

  while (current_coloring_ < total_colorings) {
    auto graph = build_colored_kn(n, current_coloring_);
    bool satisfies = checkRamseyProperty(graph, s_, t_);
    if (!satisfies)
      counterexamples_[n].push_back(current_coloring_);

    if (verbose) {
      std::cout << "Coloring #" << current_coloring_ << (satisfies ? "  [satisfies]" : "  [counterexample]") << "\n";
      graph.print();
      std::cout << "Press Enter to continue ... ";
      std::cin.ignore();
    }
    ++current_coloring_;
  }
  current_coloring_ = 0;
}

void ExhaustiveSearch::save(const std::string& filename) const {
  std::ofstream file(filename);
  if (!file.is_open())
    throw FileException(UNABLE_TO_OPEN_OUTPUT_FILE, filename);
  file << "exhaustive " << s_ << " " << t_ << " " << current_coloring_ << "\n";
  save_common(file);
}

ExhaustiveSearch ExhaustiveSearch::load(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open())
    throw FileException(UNABLE_TO_OPEN_INPUT_FILE, filename);

  std::string tag;
  size_t s, t;
  uint64_t current_coloring;
  file >> tag >> s >> t >> current_coloring;

  ExhaustiveSearch search(s, t);
  search.current_coloring_ = current_coloring;
  load_common(file, search);
  return search;
}

std::string ExhaustiveSearch::name() const { return "exhaustive"; }
