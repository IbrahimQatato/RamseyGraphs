#ifndef SEARCH_STRAT_H
#define SEARCH_STRAT_H

/// @file SearchStrategy.h
/// @brief Abstract base class for Ramsey number search strategies.

#include "Color.h"
#include "Graph.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

/// @brief Abstract base class for Ramsey number search strategies.
/// @details Implements the Template Method pattern: run() provides the outer
///          search loop (increment n, check 64-bit limit, detect completion);
///          subclasses implement test_n() to fill counterexamples for one n.
///          Shared state (s_, t_, current_n_, done_, result_, counterexamples_)
///          lives here as protected members.
class SearchStrategy {
public:
  /// @brief Tests all (or a sample of) colorings of K_n and stores counterexamples.
  /// @param n Number of nodes in the complete graph to test.
  /// @param verbose If true, prints each coloring tested.
  virtual void test_n(size_t n, bool verbose) = 0;

  /// @brief Returns the list of counterexample coloring indices for K_n.
  /// @param n Number of nodes.
  /// @return Const reference to the list — empty vector if none found.
  virtual const std::vector<uint64_t>& counterexamples(size_t n) const = 0;

  /// @brief Runs the search loop until R(s,t) is found or the 64-bit limit is hit.
  /// @param verbose Passed through to test_n on each iteration.
  void run(bool verbose) {
    if (done_) {
      std::cout << "Already complete: R(" << s_ << "," << t_ << ") = " << result_ << "\n";
      return;
    }
    while (!done_) {
      size_t edges = total_edges(current_n_);
      if (edges > 64) {
        std::cout << "K_" << current_n_ << " exceeds 64-bit limit, stopping.\n";
        break;
      }
      test_n(current_n_, verbose);
      if (!counterexamples_.count(current_n_)) {
        done_ = true;
        result_ = current_n_;
        std::cout << "R(" << s_ << "," << t_ << ") = " << result_ << "\n";
      } else {
        std::cout << "  → " << counterexamples_[current_n_].size()
                  << " counterexamples — K_" << current_n_ << " not sufficient\n";
        ++current_n_;
      }
    }
  }
  /// @brief Reconstructs a specific coloring of K_n as a graph.
  /// @param n Number of nodes.
  /// @param index Bitmask index of the coloring.
  /// @return The colored undirected graph.
  virtual UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t index) const = 0;

  /// @brief Saves the current search state to a file.
  /// @param filename Path to the output file.
  /// @throws FileException if the file cannot be opened.
  virtual void save(const std::string& filename) const = 0;

  /// @brief Returns the strategy tag used in save files (e.g. "exhaustive", "random").
  virtual std::string name() const = 0;

  virtual ~SearchStrategy() = default;

  /// @brief Returns true if R(s,t) has been found.
  bool isDone() const { return done_; }

  /// @brief Returns R(s,t). Only valid if isDone() is true.
  size_t result() const { return result_; }

  /// @brief Prints a summary of search progress to the given stream.
  /// @param out Output stream (defaults to stdout).
  void printSummary(std::ostream& out = std::cout) const {
    out << name() << " — R(" << s_ << "," << t_ << ")\n";
    for (const auto& [n, indices] : counterexamples_)
      out << "  K_" << n << ": " << indices.size() << " counterexamples\n";
    if (done_)
      out << "  Result: R(" << s_ << "," << t_ << ") = " << result_ << "\n";
    else
      out << "  In progress: K_" << current_n_ << "\n";
  }

  /// @brief Returns the number of edges in K_n: n*(n-1)/2.
  static size_t total_edges(size_t n) { return n * (n - 1) / 2; }

protected:
  SearchStrategy(size_t s, size_t t, size_t start_n)
      : s_(s), t_(t), current_n_(start_n) {}

  size_t s_, t_, current_n_;
  bool done_ = false;
  size_t result_ = 0;
  std::map<size_t, std::vector<uint64_t>> counterexamples_;

  void save_common(std::ofstream& file) const {
    file << current_n_ << " " << done_ << " " << result_ << "\n";
    for (const auto& [n, indices] : counterexamples_) {
      file << n << " " << indices.size();
      for (uint64_t idx : indices) file << " " << idx;
      file << "\n";
    }
  }

  static void load_common(std::ifstream& file, SearchStrategy& s) {
    file >> s.current_n_ >> s.done_ >> s.result_;
    size_t n, count;
    while (file >> n >> count) {
      std::vector<uint64_t> indices;
      for (size_t i = 0; i < count; ++i) {
        uint64_t idx;
        file >> idx;
        indices.push_back(idx);
      }
      s.counterexamples_[n] = std::move(indices);
    }
  }
};

#endif // !SEARCH_STRAT_H
