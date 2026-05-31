#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

/// @file RandomSampler.h
/// @brief Probabilistic Ramsey search via random 2-coloring sampling.

#include "Color.h"
#include "Graph.h"
#include "RamseyChecker.h"
#include "SearchStrategy.h"
#include <cstdint>
#include <random>
#include <string>
#include <vector>

/// @brief Searches for R(s,t) by randomly sampling 2-colorings of K_n.
/// @details For each n, draws samples_per_n random bitmask colorings using
///          a 64-bit Mersenne Twister. If no counterexample is found after
///          all samples, declares result = n (probabilistic — not a proof).
///          Useful for large n where exhaustive search is infeasible.
class RandomSampler : public SearchStrategy {
public:
  /// @brief Constructs a fresh probabilistic search for R(red_size, blue_size).
  /// @param red_size Required red clique size.
  /// @param blue_size Required blue clique size.
  /// @param samples_per_n Number of random colorings to test per K_n.
  RandomSampler(size_t red_size, size_t blue_size, size_t samples_per_n = 1000);

  /// @brief Samples samples_per_n random colorings of K_n and stores counterexamples.
  /// @param n Number of nodes in the complete graph.
  /// @param verbose If true, prints each sampled coloring and waits for Enter.
  void test_n(size_t n, bool verbose) override;

  /// @brief Reconstructs a specific coloring of K_n from a bitmask index.
  /// @param n Number of nodes.
  /// @param coloring_index Bitmask encoding the coloring.
  /// @return The colored undirected graph.
  UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t coloring_index) const override;

  /// @brief Returns the list of counterexample indices found for K_n.
  /// @param n Number of nodes.
  /// @return Const reference to the list — empty vector if none found.
  const std::vector<uint64_t>& counterexamples(size_t n) const override;

  /// @brief Saves the current search state to a file.
  /// @param filename Path to the output file.
  /// @throws FileException if the file cannot be opened.
  void save(const std::string& filename) const override;

  /// @brief Loads a previously saved RandomSampler state from a file.
  /// @param filename Path to the save file.
  /// @return RandomSampler with restored state.
  /// @throws FileException if the file cannot be opened.
  static RandomSampler load(const std::string& filename);

  /// @brief Returns the strategy tag "random".
  std::string name() const override;

private:
  size_t samples_per_n_;
  std::mt19937_64 rng_;

  static std::vector<std::pair<Identifier, Identifier>> edge_order(size_t n);
  UndirectedGraph<Identifier, Color> build_colored_kn(size_t n, uint64_t mask) const;
};

#endif // !RANDOM_SAMPLER_H
