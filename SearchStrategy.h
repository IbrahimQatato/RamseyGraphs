#ifndef SEARCH_STRAT_H
#define SEARCH_STRAT_H

#include "Color.h"
#include "Graph.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class SearchStrategy {
public:
  virtual void test_n(size_t n, bool verbose) = 0;
  virtual const std::vector<uint64_t>& counterexamples(size_t n) const = 0;

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
  virtual UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t index) const = 0;
  virtual void save(const std::string& filename) const = 0;
  virtual std::string name() const = 0;
  virtual ~SearchStrategy() = default;

  bool isDone() const { return done_; }
  size_t result() const { return result_; }

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
