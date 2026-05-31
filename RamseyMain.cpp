#include "ExhaustiveSearch.h"
#include "GraphExporter.h"
#include "Options.h"
#include "RandomSampler.h"
#include "SearchStrategy.h"
#include <csignal>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

static std::unique_ptr<SearchStrategy> g_search;
static std::string g_save_file;

std::unique_ptr<SearchStrategy> load_strategy(const std::string& filename) {
  std::ifstream f(filename);
  std::string tag;
  f >> tag;
  if (tag == "random")
    return std::make_unique<RandomSampler>(RandomSampler::load(filename));
  return std::make_unique<ExhaustiveSearch>(ExhaustiveSearch::load(filename));
}

std::unique_ptr<SearchStrategy> make_strategy(const Options& opts, size_t s, size_t t, const std::string& save_file) {
  if (std::filesystem::exists(save_file))
    return load_strategy(save_file);
  if (opts.strategy == "random")
    return std::make_unique<RandomSampler>(s, t, opts.samples);
  return std::make_unique<ExhaustiveSearch>(s, t);
}

void signal_handler(int) {
  if (g_search)
    g_search->save(g_save_file);
  std::cout << "\nSaved to " << g_save_file << "\n";
  std::exit(0);
}

int main(int argc, char** argv) {
  args_t arguments(argv + 1, argv + argc);
  Options opts = process_options(arguments);
  if (!opts.valid)
    return EXIT_CODE_FAILURE;

  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  std::filesystem::create_directories("data");

  if (opts.check_mode) {
    g_save_file = opts.check_file;
    g_search = load_strategy(opts.check_file);

    g_search->printSummary();
    std::cout << "\nCommands: list <n>  |  show <n> <index>  |  export <n> <index> <file.dot>  |  quit\n";

    std::string line;
    std::cin.ignore();
    while (std::getline(std::cin, line)) {
      std::istringstream stringstream(line);
      std::string command;
      stringstream >> command;

      if (command == "quit") {
        break;
      } else if (command == "list") {
        size_t n;
        stringstream >> n;
        const auto& indices = g_search->counterexamples(n);
        if (indices.empty()) {
          std::cout << "No counterexamples for K_" << n << "\n";
        } else {
          for (size_t i = 0; i < indices.size(); ++i)
            std::cout << "  [" << i << "] coloring index " << indices[i] << "\n";
        }
      } else if (command == "show") {
        size_t n, index;
        stringstream >> n >> index;
        const auto& indices = g_search->counterexamples(n);
        if (index >= indices.size()) {
          std::cout << "Index out of range\n";
        } else {
          auto graph = g_search->getColoring(n, indices[index]);
          graph.print();
          graph.edges().printMatrix();
        }
      } else if (command == "export") {
        size_t n, index;
        std::string dotfile;
        stringstream >> n >> index >> dotfile;
        const auto& indices = g_search->counterexamples(n);
        if (index >= indices.size()) {
          std::cout << "Index out of range\n";
        } else {
          auto graph = g_search->getColoring(n, indices[index]);
          exportDOT(graph, dotfile);
          std::cout << "Exported to " << dotfile << "\n";
        }
      } else {
        std::cout << "Unknown command\n";
      }
    }
  } else {
    size_t s, t;
    std::cout << "Enter s and t for R(s,t): ";
    std::cin >> s >> t;

    std::string save_file = "data/R_" + std::to_string(s) + "_" + std::to_string(t) + ".txt";
    g_save_file = save_file;

    g_search = make_strategy(opts, s, t, save_file);
    g_search->run(opts.verbose);
    g_search->save(save_file);
  }
  return EXIT_CODE_SUCCESS;
}
