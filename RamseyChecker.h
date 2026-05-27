#ifndef RAMSEY_CHECKER_H
#define RAMSEY_CHECKER_H
#include "Color.h"
#include "Graph.h"
#include "Utils.h"
#include <numeric>
#include <vector>

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

template <GraphData NData>
bool checkRamseyProperty(const UndirectedGraph<NData, Color>& graph, size_t s, size_t t) {
  return hasClique(graph, s, Color::Red) || hasClique(graph, t, Color::Blue);
}

#endif // !RAMSEY_CHECKER_H
