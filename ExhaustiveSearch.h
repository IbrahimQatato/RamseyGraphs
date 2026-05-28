#ifndef EXHAUSTIVE_SEARCH_H
#define EXHAUSTIVE_SEARCH_H

/// @file ExhaustiveSearch.h
/// @brief Exhaustive 2-coloring search for Ramsey numbers R(s,t).

#include "Color.h"
#include "Graph.h"
#include "RamseyChecker.h"
#include <cstdint>
#include <map>
#include <string>
#include <vector>

/// @brief Searches for the Ramsey number R(s,t) by exhaustively testing all
///        2-colorings of K_n for increasing n until all colorings satisfy the
///        Ramsey property.
class ExhaustiveSearch {
public:
  /// @brief Constructs a fresh search for R(s,t).
  /// @param s Required red clique size.
  /// @param t Required blue clique size.
  ExhaustiveSearch(size_t s, size_t t);

  /// @brief Runs the exhaustive search until R(s,t) is found or the 64-bit
  ///        coloring limit is exceeded.
  /// @param verbose If true, prints every coloring and waits for Enter.
  void run(bool verbose);

  /// @brief Reconstructs a specific coloring of K_n as a graph.
  /// @param n Number of nodes in the complete graph.
  /// @param coloring_index Bitmask index of the coloring.
  /// @return The colored undirected graph.
  UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t coloring_index) const;

  /// @brief Returns the list of counterexample coloring indices for K_n.
  /// @param n Number of nodes.
  /// @return Const reference to the list of bitmask indices — empty if none found.
  const std::vector<uint64_t>& counterexamples(size_t n) const;

  /// @brief Returns true if R(s,t) has been found.
  bool isDone() const;

  /// @brief Returns R(s,t). Only valid if isDone() is true.
  size_t result() const;

  /// @brief Prints a summary of search progress to the given stream.
  /// @param out Output stream (defaults to stdout).
  void printSummary(std::ostream& out = std::cout) const;

  /// @brief Saves the current search state to a file for later resumption.
  /// @param filename Path to the save file.
  /// @throws FileException if the file cannot be opened.
  void save(const std::string& filename) const;

  /// @brief Loads a previously saved search state from a file.
  /// @param filename Path to the save file.
  /// @return ExhaustiveSearch object with restored state.
  /// @throws FileException if the file cannot be opened.
  static ExhaustiveSearch load(const std::string& filename);

private:
  size_t s_;
  size_t t_;
  size_t current_n_;
  uint64_t current_coloring_;
  bool done_;
  size_t result_;
  std::map<size_t, std::vector<uint64_t>> counterexamples_;

  /// @brief Returns the number of edges in K_n: n*(n-1)/2.
  static size_t total_edges(size_t n);

  /// @brief Returns all edges of K_n in a fixed order for consistent bitmask mapping.
  static std::vector<std::pair<Identifier, Identifier>> edge_order(size_t n);

  /// @brief Builds K_n with edges colored according to the bitmask.
  /// @param n Number of nodes.
  /// @param mask Bitmask — bit i=0 means Red, bit i=1 means Blue for edge i.
  UndirectedGraph<Identifier, Color> build_colored_kn(size_t n, uint64_t mask) const;
};

#endif // !EXHAUSTIVE_SEARCH_H
