#ifndef GRAPH_EXPORTER_H
#define GRAPH_EXPORTER_H

/// @file GraphExporter.h
/// @brief DOT format export for 2-colored undirected graphs.

#include "Color.h"
#include "Graph.h"
#include <ostream>
#include <string>

constexpr char DOT_COLOR_RED[] = "red";
constexpr char DOT_COLOR_BLUE[] = "blue";

/// @brief Writes a 2-colored graph in Graphviz DOT format to a stream.
/// @details Red edges are written as `color=red`, blue as `color=blue`.
///          Output can be rendered with: `dot -Tpng file.dot -o file.png`
/// @param graph The colored undirected graph to export.
/// @param out Output stream (defaults to stdout).
void exportDOT(const UndirectedGraph<Identifier, Color>& graph, std::ostream& out = std::cout);

/// @brief Writes a 2-colored graph in Graphviz DOT format to a file.
/// @param graph The colored undirected graph to export.
/// @param filename Path to the output .dot file.
/// @throws FileException if the file cannot be opened.
void exportDOT(const UndirectedGraph<Identifier, Color>& graph, const std::string& filename);

#endif // !GRAPH_EXPORTER_H
