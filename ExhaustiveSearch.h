#ifndef EXHAUSTIVE_SEARCH_H
#define EXHAUSTIVE_SEARCH_H

#include "Color.h"
#include "Graph.h"
#include "RamseyChecker.h"
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class ExhaustiveSearch {
public:
  ExhaustiveSearch(size_t s, size_t t);

  void run(bool verbose);

  UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t coloring_index) const;

  const std::vector<uint64_t>& counterexamples(size_t n) const;

  bool isDone() const;
  size_t result() const; 

  void save(const std::string& filename) const;
  static ExhaustiveSearch load(const std::string& filename);

private:
  size_t s_;
  size_t t_;
  size_t current_n_;
  uint64_t current_coloring_;
  bool done_;
  size_t result_;
  std::map<size_t, std::vector<uint64_t>> counterexamples_;

  static size_t total_edges(size_t n);
  static std::vector<std::pair<Identifier, Identifier>> edge_order(size_t n);
  UndirectedGraph<Identifier, Color> build_colored_kn(size_t n, uint64_t mask) const;
};

#endif // !EXHAUSTIVE_SEARCH_H
