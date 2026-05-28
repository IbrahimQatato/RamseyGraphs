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

void ExhaustiveSearch::printSummary(std::ostream& out) const {
  out << "\nLoaded R(" << s_ << "," << t_ << ") search\n";
  for (const auto& [n, indices] : counterexamples_)
    out << "  K_" << n << ": " << indices.size() << " counterexamples\n";
  if (isDone())
    out << "  Result: R(" << s_ << "," << t_ << ") = " << result_ << "\n";
  else
    out << "  In progress: K_" << current_n_ << " (coloring " << current_coloring_ << ")\n";
}

const std::vector<uint64_t>& ExhaustiveSearch::counterexamples(size_t n) const {
  static const std::vector<uint64_t> empty;
  auto it = counterexamples_.find(n);
  return it != counterexamples_.end() ? it->second : empty;
}

void ExhaustiveSearch::run(bool verbose) {
  if (done_) {
    std::cout << "Already complete: R(" << s_ << "," << t_ << ") = " << result_ << "\n";
    return;
  }
  while (!done_) {
    size_t no_of_edges = total_edges(current_n_);

    if (no_of_edges > 64) {
      std::cout << "K_" << current_n_ << " has " << no_of_edges
                << " edges — exceeds 64-bit limit, stopping.\n";
      break;
    }

    uint64_t total_colorings = 1ULL << no_of_edges;
    bool all_satisfy = counterexamples_.count(current_n_) == 0;

    std::cout << "\nTesting K_" << current_n_ << " (" << no_of_edges << " edges, " << total_colorings
              << " colorings)...\n";

    while (current_coloring_ < total_colorings) {
      auto graph = build_colored_kn(current_n_, current_coloring_);
      bool satisfies = checkRamseyProperty(graph, s_, t_);
      if (!satisfies) {
        counterexamples_[current_n_].push_back(current_coloring_);
        all_satisfy = false;
      }

      if (verbose) {
        std::cout << "Coloring #" << current_coloring_ << (satisfies ? "  [satisfies]" : "  [counterexample]")
                  << "\n";
        graph.print();
        std::cout << "Press Enter to continue ... ";
        std::cin.ignore();
      }
      ++current_coloring_;
    }

    if (all_satisfy) {
      result_ = current_n_;
      done_ = true;
      std::cout << "R(" << s_ << "," << t_ << ") = " << result_ << "\n";
    } else {
      std::cout << "  → " << counterexamples_[current_n_].size() << " counterexamples — K_" << current_n_
                << " not sufficient\n";
      ++current_n_;
      current_coloring_ = 0;
    }
  }
}

void ExhaustiveSearch::save(const std::string& filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw FileException(UNABLE_TO_OPEN_OUTPUT_FILE, filename);
  }
  file << s_ << " " << t_ << std::endl;
  file << current_n_ << " " << current_coloring_ << " " << done_ << " " << result_ << std::endl;
  for (const auto& [n, indices] : counterexamples_) {
    file << n << " " << indices.size();
    for (uint64_t index : indices)
      file << " " << index;
    file << std::endl;
  }
}

ExhaustiveSearch ExhaustiveSearch::load(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open())
    throw FileException(UNABLE_TO_OPEN_INPUT_FILE, filename);

  size_t s, t;
  file >> s >> t;

  ExhaustiveSearch search(s, t);
  file >> search.current_n_ >> search.current_coloring_ >> search.done_ >> search.result_;

  size_t n, count;
  while (file >> n >> count) {
    std::vector<uint64_t> indices;
    for (size_t i = 0; i < count; ++i) {
      uint64_t index;
      file >> index;
      indices.push_back(index);
    }
    search.counterexamples_[n] = std::move(indices);
  }

  return search;
}
