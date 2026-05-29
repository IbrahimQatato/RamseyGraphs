#ifndef RANDOM_SAMPLER_H
#define RANDOM_SAMPLER_H

#include "Color.h"
#include "Graph.h"
#include "RamseyChecker.h"
#include "SearchStrategy.h"
#include <cstdint>
#include <map>
#include <random>
#include <string>
#include <vector>

class RandomSampler : public SearchStrategy {
public:
  RandomSampler(size_t s, size_t t, size_t samples_per_n = 1000);

  void run(bool verbose) override;

  UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t coloring_index) const override;

  const std::vector<uint64_t>& counterexamples(size_t n) const override;

  bool isDone() const override;

  size_t result() const override;

  void printSummary(std::ostream& out = std::cout) const override;

  void save(const std::string& filename) const override;

  static RandomSampler load(const std::string& filename);

  std::string name() const override;

private:
  size_t s_;
  size_t t_;
  size_t current_n_;
  size_t samples_per_n_;
  bool done_;
  size_t result_;
  std::map<size_t, std::vector<uint64_t>> counterexamples_;
  std::mt19937_64 rng_;

  static std::vector<std::pair<Identifier, Identifier>> edge_order(size_t n);
  UndirectedGraph<Identifier, Color> build_colored_kn(size_t n, uint64_t mask) const;
};

#endif // !RANDOM_SAMPLER_H
