/// @file Tests.cpp
/// @brief Unit and integration tests for the Ramsey Analyzer.
#include "ExhaustiveSearch.h"
#include "GraphExporter.h"
#include "RandomSampler.h"
#include "RamseyChecker.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

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


void test_has_clique() {
  auto g1 = make_uniform_kn(3, Color::Red);
  expect(hasClique(g1, 3, Color::Red), "all-red K_3 has red K_3");
  expect(!hasClique(g1, 3, Color::Blue), "all-red K_3 has no blue K_3");

  auto g2 = make_uniform_kn(4, Color::Blue);
  expect(hasClique(g2, 4, Color::Blue), "all-blue K_4 has blue K_4");
  expect(hasClique(g2, 3, Color::Blue), "all-blue K_4 has blue K_3");
  expect(!hasClique(g2, 3, Color::Red), "all-blue K_4 has no red K_3");

  auto g3 = make_uniform_kn(3, Color::Red);
  expect(hasClique(g3, 1, Color::Red), "k=1 always true");

  auto g4 = make_uniform_kn(2, Color::Red);
  expect(!hasClique(g4, 3, Color::Red), "k > n returns false");
}


void test_check_ramsey_property() {
  auto g1 = make_uniform_kn(6, Color::Red);
  expect(checkRamseyProperty(g1, 3, 3), "all-red K_6 satisfies R(3,3)");

  auto g2 = make_uniform_kn(6, Color::Blue);
  expect(checkRamseyProperty(g2, 3, 3), "all-blue K_6 satisfies R(3,3)");

  ExhaustiveSearch search(3, 3);
  auto g3 = search.getColoring(5, 220);
  expect(!checkRamseyProperty(g3, 3, 3), "known K_5 counterexample does not satisfy R(3,3)");
}


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


void test_random_sampler() {
  RandomSampler sampler(3, 3, 5000);
  expect(sampler.name() == "random", "RandomSampler name is 'random'");

  sampler.run(false);
  expect(sampler.isDone(), "RandomSampler completes");
  expect(sampler.result() >= 3, "RandomSampler result is plausible");

  std::filesystem::create_directories("data");
  const std::string file = "data/test_random_save.txt";
  sampler.save(file);
  RandomSampler loaded = RandomSampler::load(file);
  expect(loaded.isDone() == sampler.isDone(), "loaded RandomSampler isDone matches");
  expect(loaded.result() == sampler.result(), "loaded RandomSampler result matches");
  expect(loaded.counterexamples(5).size() == sampler.counterexamples(5).size(),
         "loaded RandomSampler counterexamples match");
  std::filesystem::remove(file);

  std::unique_ptr<SearchStrategy> ptr = std::make_unique<RandomSampler>(3, 3, 1000);
  ptr->run(false);
  expect(ptr->isDone(), "RandomSampler via SearchStrategy ptr completes");
  expect(ptr->name() == "random", "SearchStrategy ptr dispatches name() to RandomSampler");
}


void test_graph_exporter() {
  ExhaustiveSearch es(3, 3);
  auto graph = es.getColoring(3, 2);

  std::ostringstream oss;
  exportDOT(graph, oss);
  std::string out = oss.str();

  expect(out.find("graph G {") != std::string::npos, "DOT has graph header");
  expect(out.find("node [shape=circle]") != std::string::npos, "DOT has node style");
  expect(out.find("color=red") != std::string::npos, "DOT has red edges");
  expect(out.find("color=blue") != std::string::npos, "DOT has blue edges");
  expect(out.find("--") != std::string::npos, "DOT has undirected edge syntax");
}


void test_save_tags() {
  std::filesystem::create_directories("data");
  const std::string es_file = "data/test_es_tag.txt";
  const std::string rs_file = "data/test_rs_tag.txt";

  ExhaustiveSearch es(3, 3);
  es.run(false);
  es.save(es_file);

  std::ifstream f1(es_file);
  std::string tag1;
  f1 >> tag1;
  expect(tag1 == "exhaustive", "ExhaustiveSearch saves 'exhaustive' tag");

  RandomSampler rs(3, 3, 1000);
  rs.run(false);
  rs.save(rs_file);

  std::ifstream f2(rs_file);
  std::string tag2;
  f2 >> tag2;
  expect(tag2 == "random", "RandomSampler saves 'random' tag");

  std::filesystem::remove(es_file);
  std::filesystem::remove(rs_file);
}

void test_r24_result() {
  ExhaustiveSearch search(2, 4);
  search.run(false);
  expect(search.isDone(), "R(2,4) search completes");
  expect(search.result() == 4, "R(2,4) = 4");
}


void test_mid_search_resume() {
  std::filesystem::create_directories("data");
  const std::string file = "data/test_mid_resume.txt";

  ExhaustiveSearch search(3, 3);
  search.save(file);

  ExhaustiveSearch resumed = ExhaustiveSearch::load(file);
  resumed.run(false);
  expect(resumed.isDone(), "mid-search resume completes");
  expect(resumed.result() == 6, "mid-search resume gives correct result");

  std::filesystem::remove(file);
}

// --- main ---

int main() {
  std::cout << "hasClique:\n";
  test_has_clique();

  std::cout << "\ncheckRamseyProperty:\n";
  test_check_ramsey_property();

  std::cout << "\nIntegration: R(3,3):n";
  test_r33_result();

  std::cout << "\nIntegration: save/load:\n";
  test_save_load();

  std::cout << "\nIntegration: resume:\n";
  test_resume_already_done();

  std::cout << "\nRandomSampler:\n";
  test_random_sampler();

  std::cout << "\nGraphExporter:\n";
  test_graph_exporter();

  std::cout << "\nSave tags:\n";
  test_save_tags();

  std::cout << "\nIntegration: R(2,4):\n";
  test_r24_result();

  std::cout << "\nIntegration: mid-search resume:\n";
  test_mid_search_resume();

  std::cout << "\n--- Results: " << passed << " passed, " << failed << " failed ---\n";
  return failed > 0 ? 1 : 0;
}
