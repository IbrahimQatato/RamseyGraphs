#include "Graph.h"
#include <iostream>
#include <string>

int main() {
  // Use string for NData and EData as they support << and >> [1, 3]
  DirectedGraph<std::string, std::string> dg;
  std::cout << "constructed OK\n";

  // Use Case: Adding nodes (implicit and explicit identifiers) [4, 5]
  dg.nodes().add("zero");   // ID 0
  dg.nodes().add(1, "one"); // ID 1
  dg.nodes().add("two");    // ID 2
  std::cout << "size: " << dg.nodes().size() << "\n";
  std::cout << dg.nodes();
  dg.edges().printMatrix();

  // Use Case: Adding edges [6, 7]
  dg.edges().add(0, 0, 1, "zero-one"); // ID 0, Source 0, Target 1
  dg.edges().add(0, 2, "zero-two");    // Implicit ID 1, Source 0, Target 2
  std::cout << dg.edges();

  // update the matrix check
  dg.edges().printMatrix();

  auto expect_throw = [](std::string label, auto fn) {
    try {
      fn();
      std::cout << "FAIL (no throw): " << label << '\n';
    } catch (const Exception& e) {
      std::cout << "OK [" << label << "] " << e.message() << '\n';
    }
  };
  expect_throw("duplicate node", [&] { dg.nodes().add(0, "again"); });
  expect_throw("skip id", [&] { dg.nodes().add(5, "skip"); });
  expect_throw("bad id", [&] { dg.nodes().add(SIZE_MAX, "bad"); });

  expect_throw("dup edge", [&] { dg.edges().add(0, 1, "again"); });
  expect_throw("bad src", [&] { dg.edges().add(9, 1, "e"); });
  expect_throw("bad tgt", [&] { dg.edges().add(0, 9, "e"); });

  // Use Case: Serialization [8-10]
  // Should print nodes first, then edges, in ascending ID order [8]
  std::cout << "--- Full Graph ---" << std::endl;
  dg.print();

  std::cout << "--- Iterator Tests ---" << std::endl;
  for (const auto& node : dg.nodes())
    std::cout << node << '\n';
  // Test 2: manual iterator
  auto it = dg.nodes().begin();
  std::cout << (*it).getId() << '\n'; // 0
  ++it;
  std::cout << it->getData() << '\n'; // "one"

  // Test 3: iterator on const graph
  const auto& cg = dg;
  for (const auto& node : cg.nodes())
    std::cout << node.getId() << ' ';
  std::cout << '\n';

  for (const auto& edge : dg.edges())
    std::cout << edge << '\n';

  std::cout << "--- Test Imports ---" << "\n";
  std::string input = R"(
                          node (0 {zero})
                          node (1 {one})
                          edge (0)-[0 {zero-one}]->(1)
                          node (2 {two})
                          edge (0)-[1 {zero-two}]->(2)
                        )";

  DirectedGraph<std::string, std::string> g;
  std::istringstream ss(input);
  g.import(ss);

  // Verify what was imported
  std::cout << "nodes: " << g.nodes().size() << '\n'; // 3
  std::cout << "edges: " << g.edges().size() << '\n'; // 2
  g.print();

  DirectedGraph<std::string, std::string> imported_dg;

  imported_dg.import("graph.txt");
  std::cout << g.nodes().size() << '\n'; // 3

  // Error path — file doesn't exist
  try {
    imported_dg.import("nonexistent.txt");
    std::cout << "FAIL: should have thrown\n";
  } catch (const Exception& e) {
    std::cout << "OK: " << e.message() << '\n';
  }

  std::cout << "--- Test copy & move ---" << "\n";

  DirectedGraph<std::string, std::string> a;
  a.nodes().add("zero");
  a.nodes().add("one");
  a.edges().add(0, 1, "zero-one");

  // Copy construct
  DirectedGraph<std::string, std::string> b = a;

  // Modify b — must not affect a
  b.nodes().add("two");
  b.nodes().get(0).getData() = "ZERO";

  std::cout << "a nodes: " << a.nodes().size() << '\n'; // 2
  std::cout << "b nodes: " << b.nodes().size() << '\n'; // 3
  std::cout << a.nodes()[0].getData() << '\n';          // zero (unchanged)
  std::cout << b.nodes()[0].getData() << '\n';          // ZERO

  // Check b's matrix is independent
  b.edges().printMatrix();
  // 3x3 matrix, b has its own copy
  // Get a reference to a node BEFORE the move
  auto& node_ref = a.nodes()[0];

  // Move
  DirectedGraph<std::string, std::string> c = std::move(a);

  // b has everything
  std::cout << c.nodes().size() << '\n'; // 2
  std::cout << c.edges().size() << '\n'; // 1

  // The reference is still valid — same address
  std::cout << node_ref.getData() << '\n';           // zero
  std::cout << (&node_ref == &c.nodes()[0]) << '\n'; // 1 (true) — same address in memory

  // a is empty but valid — can still call methods
  std::cout << a.nodes().size() << '\n'; // 0
  g.clear();

  std::cout << g.nodes().size() << '\n'; // 0
  std::cout << g.edges().size() << '\n'; // 0

  // Can still use the graph after clearing
  g.nodes().add("fresh");
  std::cout << g.nodes().size() << '\n'; // 1
  g.edges().printMatrix();               // 1x1 matrix of dashes
  return 0;
}
