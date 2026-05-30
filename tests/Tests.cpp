#include "ExhaustiveSearch.h"
#include "RandomSampler.h"
#include "RamseyChecker.h"
#include <filesystem>
#include <iostream>
#include <string>

// --- test helpers ---

static int passed = 0;
static int failed = 0;

void expect(bool condition, const std::string& name) {
  if (condition) {
    std::cout << "PASS: " << name << "\n";
    ++passed;
  } else {
    std::cout << "FAIL: " << name << "\n";
    ++failed;
  }
}

// builds a K_n with all edges set to given color
UndirectedGraph<Identifier, Color> make_uniform_kn(size_t n, Color color) {
  UndirectedGraph<Identifier, Color> graph;
  for (Identifier i = 0; i < n; ++i)
    graph.nodes().add(i, i);
  Identifier edge_id = 0;
  for (Identifier i = 0; i < n; ++i)
    for (Identifier j = i + 1; j < n; ++j)
      graph.edges().add(edge_id++, i, j, color);
  return graph;
}

// --- hasClique tests ---

void test_has_clique() {
  // all red K_3 — has red K_3
  auto g1 = make_uniform_kn(3, Color::Red);
  expect(hasClique(g1, 3, Color::Red), "all-red K_3 has red K_3");
  expect(!hasClique(g1, 3, Color::Blue), "all-red K_3 has no blue K_3");

  // all blue K_4 — has blue K_4 and blue K_3, no red clique
  auto g2 = make_uniform_kn(4, Color::Blue);
  expect(hasClique(g2, 4, Color::Blue), "all-blue K_4 has blue K_4");
  expect(hasClique(g2, 3, Color::Blue), "all-blue K_4 has blue K_3");
  expect(!hasClique(g2, 3, Color::Red), "all-blue K_4 has no red K_3");

  // k=1 — always true (single node is trivially a clique)
  auto g3 = make_uniform_kn(3, Color::Red);
  expect(hasClique(g3, 1, Color::Red), "k=1 always true");

  // k > n — impossible
  auto g4 = make_uniform_kn(2, Color::Red);
  expect(!hasClique(g4, 3, Color::Red), "k > n returns false");
}

// --- checkRamseyProperty tests ---

void test_check_ramsey_property() {
  // all red K_6 — has red K_3, satisfies R(3,3)
  auto g1 = make_uniform_kn(6, Color::Red);
  expect(checkRamseyProperty(g1, 3, 3), "all-red K_6 satisfies R(3,3)");

  // all blue K_6 — has blue K_3, satisfies R(3,3)
  auto g2 = make_uniform_kn(6, Color::Blue);
  expect(checkRamseyProperty(g2, 3, 3), "all-blue K_6 satisfies R(3,3)");

  // known R(3,3) counterexample on K_5: coloring index 220
  ExhaustiveSearch search(3, 3);
  auto g3 = search.getColoring(5, 220);
  expect(!checkRamseyProperty(g3, 3, 3), "known K_5 counterexample does not satisfy R(3,3)");
}

// --- integration tests ---

void test_r33_result() {
  ExhaustiveSearch search(3, 3);
  search.run(false);
  expect(search.isDone(), "R(3,3) search completes");
  expect(search.result() == 6, "R(3,3) = 6");
}

void test_save_load() {
  std::filesystem::create_directories("data");
  const std::string file = "data/test_save.txt";

  ExhaustiveSearch search(3, 3);
  search.run(false);
  search.save(file);

  ExhaustiveSearch loaded = ExhaustiveSearch::load(file);
  expect(loaded.isDone(), "loaded search is done");
  expect(loaded.result() == 6, "loaded result is 6");
  expect(loaded.counterexamples(5).size() == search.counterexamples(5).size(),
         "loaded counterexamples match");

  std::filesystem::remove(file);
}

void test_resume_already_done() {
  ExhaustiveSearch search(3, 3);
  search.run(false);
  // running again on a completed search should report done immediately
  search.run(false);
  expect(search.isDone(), "re-running completed search stays done");
  expect(search.result() == 6, "result unchanged after re-run");
}

// --- RandomSampler tests ---

void test_random_sampler() {
  // name() returns the tag used in save files
  RandomSampler sampler(3, 3, 5000);
  expect(sampler.name() == "random", "RandomSampler name is 'random'");

  // after run, search declares a result — exact value is probabilistic
  // but must be >= max(s,t) = 3
  sampler.run(false);
  expect(sampler.isDone(), "RandomSampler completes");
  expect(sampler.result() >= 3, "RandomSampler result is plausible");

  // save/load round-trip: loaded state matches original
  std::filesystem::create_directories("data");
  const std::string file = "data/test_random_save.txt";
  sampler.save(file);
  RandomSampler loaded = RandomSampler::load(file);
  expect(loaded.isDone() == sampler.isDone(), "loaded RandomSampler isDone matches");
  expect(loaded.result() == sampler.result(), "loaded RandomSampler result matches");
  expect(loaded.counterexamples(5).size() == sampler.counterexamples(5).size(),
         "loaded RandomSampler counterexamples match");
  std::filesystem::remove(file);

  // polymorphism: SearchStrategy pointer calls RandomSampler methods via vtable
  std::unique_ptr<SearchStrategy> ptr = std::make_unique<RandomSampler>(3, 3, 1000);
  ptr->run(false);
  expect(ptr->isDone(), "RandomSampler via SearchStrategy ptr completes");
  expect(ptr->name() == "random", "SearchStrategy ptr dispatches name() to RandomSampler");
}

// --- main ---

int main() {
  std::cout << "=== hasClique ===\n";
  test_has_clique();

  std::cout << "\n=== checkRamseyProperty ===\n";
  test_check_ramsey_property();

  std::cout << "\n=== Integration: R(3,3) ===\n";
  test_r33_result();

  std::cout << "\n=== Integration: save/load ===\n";
  test_save_load();

  std::cout << "\n=== Integration: resume ===\n";
  test_resume_already_done();

  std::cout << "\n=== RandomSampler ===\n";
  test_random_sampler();

  std::cout << "\n--- Results: " << passed << " passed, " << failed << " failed ---\n";
  return failed > 0 ? 1 : 0;
}
