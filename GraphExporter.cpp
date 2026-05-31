#include "GraphExporter.h"
#include "Utils.h"
#include <fstream>

static std::string toDOTColor(Color color) { return color == Color::Red ? "red" : "blue"; }

void exportDOT(const UndirectedGraph<Identifier, Color>& graph, std::ostream& out) {
  out << "graph G {\n";
  out << "    node [shape=circle]\n";
  for (const auto& edge : graph.edges()) {
    out << "    " << edge.getSource() << " -- " << edge.getTarget();
    out << " [color=" << toDOTColor(edge.getData()) << "]\n";
  }
  out << "}\n";
}

void exportDOT(const UndirectedGraph<Identifier, Color>& graph, const std::string& filename) {
  std::ofstream file(filename);
  if (!file.is_open())
    throw FileException(UNABLE_TO_OPEN_OUTPUT_FILE, filename);
  exportDOT(graph, file);
}
