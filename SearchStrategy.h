#ifndef SEARCH_STRAT_H
#define SEARCH_STRAT_H

#include "Color.h"
#include "Graph.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class SearchStrategy {
public:
  virtual void run(bool verbose) = 0;
  virtual bool isDone() const = 0;
  virtual size_t result() const = 0;
  virtual const std::vector<uint64_t>& counterexamples(size_t n) const = 0;
  virtual void printSummary(std::ostream& out = std::cout) const = 0;
  virtual UndirectedGraph<Identifier, Color> getColoring(size_t n, uint64_t index) const = 0;
  virtual void save(const std::string& filename) const = 0;
  virtual std::string name() const = 0;
  virtual ~SearchStrategy() = default;
};

#endif // !SEARCH_STRAT_H
