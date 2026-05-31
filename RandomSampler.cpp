#include "RandomSampler.h"
#include "RamseyChecker.h"
#include "Utils.h"
#include <string>

RandomSampler::RandomSampler(size_t s, size_t t, size_t samples_per_n)
    : SearchStrategy(s, t, std::max(s, t)), samples_per_n_(samples_per_n), rng_(std::random_device{}()) {}

std::vector<std::pair<Identifier, Identifier>> RandomSampler::edge_order(size_t n) {
  std::vector<std::pair<Identifier, Identifier>> result;
  for (Identifier i = 0; i < n; ++i) {
    for (Identifier j = i + 1; j < n; ++j) {
      result.push_back({i, j});
    }
  }
  return result;
}

UndirectedGraph<Identifier, Color> RandomSampler::build_colored_kn(size_t n, uint64_t mask) const {
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

void RandomSampler::test_n(size_t n, bool verbose) { //TODO add verbose instructions
  size_t edges = total_edges(n);
  std::uniform_int_distribution<uint64_t> dist(0, (1ULL << edges) - 1);
  for (size_t i = 0; i < samples_per_n_; ++i) {
    uint64_t mask = dist(rng_);
    auto graph = build_colored_kn(n, mask);
    if (!checkRamseyProperty(graph, s_, t_))
      counterexamples_[n].push_back(mask);
  }
  std::cout << "\nSampled K_" << n << " (" << samples_per_n_ << " samples)\n";
}

const std::vector<uint64_t>& RandomSampler::counterexamples(size_t n) const {
  static const std::vector<uint64_t> empty;
  auto it = counterexamples_.find(n);
  return it != counterexamples_.end() ? it->second : empty;
}

std::string RandomSampler::name() const { return "random"; }

UndirectedGraph<Identifier, Color> RandomSampler::getColoring(size_t n, uint64_t coloring_index) const {
  return build_colored_kn(n, coloring_index);
}

void RandomSampler::save(const std::string& filename) const {
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw FileException(UNABLE_TO_OPEN_OUTPUT_FILE, filename);
  }
  file << "random " << s_ << " " << t_ << " " << samples_per_n_ << "\n";
  save_common(file);
}

RandomSampler RandomSampler::load(const std::string& filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    FileException(UNABLE_TO_OPEN_INPUT_FILE, filename);
  }
  std::string tag;
  size_t s, t, samples_per_n;
  file >> tag >> s >> t >> samples_per_n;
  RandomSampler search(s, t);
  search.samples_per_n_ = samples_per_n;
  load_common(file, search);
  return search;
}
