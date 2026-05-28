#ifndef RAMSEY_CHECKER_H
#define RAMSEY_CHECKER_H

/// @file RamseyChecker.h
/// @brief Clique detection and Ramsey property checking for 2-colored graphs.

#include "Color.h"
#include "Graph.h"
#include "Utils.h"
#include <numeric>
#include <vector>

/// @brief Advances index_vector to the next combination of k elements from n.
/// @details Uses the standard combination enumeration algorithm — finds the
///          rightmost index with room to increment, increments it, resets
///          everything to its right. Called repeatedly by hasClique.
/// @param index_vector Current combination indices, modified in place.
/// @param n Total number of elements to choose from.
/// @return true if a next combination exists, false if all exhausted.
inline bool advance_combination(std::vector<size_t>& index_vector, size_t n) {
  size_t k = index_vector.size();

  int i = static_cast<int>(k) - 1;
  while (i >= 0 && index_vector[i] == n - k + i)
    --i;

  if (i < 0)
    return false;

  ++index_vector[i];
  for (size_t j = i + 1; j < k; ++j)
    index_vector[j] = index_vector[j - 1] + 1;

  return true;
}

/// @brief Checks whether a 2-colored graph contains a monochromatic clique.
/// @details Tries every C(n, clique_size) subset of nodes using combination
///          enumeration. Returns true on the first subset where all edges
///          between members have the given color.
/// @tparam NData Node data type — must satisfy GraphData concept.
/// @param graph The colored undirected graph to search.
/// @param clique_size Size of clique to look for.
/// @param color Color to check (Red or Blue).
/// @return true if a monochromatic clique of given size and color exists.
template <GraphData NData>
bool hasClique(const UndirectedGraph<NData, Color>& graph, size_t clique_size, Color color) {
  std::vector<Identifier> nodes;
  for (const auto& node : graph.nodes())
    nodes.push_back(node.getId());

  size_t n = nodes.size();
  if (n < clique_size)
    return false;

  std::vector<size_t> index_vector(clique_size);
  std::iota(index_vector.begin(), index_vector.end(), 0);

  while (true) {
    bool is_clique = true;
    for (size_t i = 0; i < clique_size && is_clique; ++i) {
      for (size_t j = i + 1; j < clique_size && is_clique; ++j) {
        Identifier node1 = nodes[index_vector[i]];
        Identifier node2 = nodes[index_vector[j]];

        if (!graph.edges().exists(node1, node2) ||
            graph.edges().get(node1, node2).getData() != color)
          is_clique = false;
      }
    }

    if (is_clique)
      return true;

    if (!advance_combination(index_vector, n))
      return false;
  }
}

/// @brief Checks whether a 2-colored graph satisfies the Ramsey property for R(s,t).
/// @details A coloring satisfies R(s,t) if it contains a red clique of size s
///          OR a blue clique of size t. A coloring that satisfies neither is a
///          counterexample — proof that more nodes are needed.
/// @tparam NData Node data type — must satisfy GraphData concept.
/// @param graph The colored undirected graph to check.
/// @param s Required size of red clique.
/// @param t Required size of blue clique.
/// @return true if the coloring contains a red K_s or blue K_t.
template <GraphData NData>
bool checkRamseyProperty(const UndirectedGraph<NData, Color>& graph, size_t s, size_t t) {
  return hasClique(graph, s, Color::Red) || hasClique(graph, t, Color::Blue);
}

#endif // !RAMSEY_CHECKER_H
