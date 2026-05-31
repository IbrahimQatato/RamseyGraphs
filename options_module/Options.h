#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>


// Define a type alias for a vector of strings (args_t)
using args_t = std::vector<std::string>;
using iterator_t = args_t::const_iterator;

constexpr char OPTION_VERBOSE_SHORT = 'v';
constexpr char OPTION_VERBOSE_LONG[] = "verbose";

constexpr char VALUE_OPTION_RESUME_LONG[] = "resume";
constexpr char VALUE_OPTION_CHECK_LONG[] = "check";
constexpr char VALUE_OPTION_STRATEGY_LONG[] = "strategy";
constexpr char VALUE_OPTION_SAMPLES_LONG[] = "samples";

constexpr int EXIT_CODE_SUCCESS = 0;
constexpr int EXIT_CODE_FAILURE = 1;

struct Options {
  bool verbose = false;
  bool check_mode = false;
  std::string resume_file;
  std::string check_file;
  std::string strategy = "exhaustive";
  size_t samples = 10000;
  bool valid = true;
};

void print_unkown(const std::string& option);
void read_value_option(const std::string& option, iterator_t& it,
                       iterator_t end, std::string& out, bool& valid);
void process_short_options(const std::string& arg, iterator_t& it,
                           iterator_t end, Options& opts);
void process_long_options(const std::string& arg, iterator_t& it,
                          iterator_t end, Options& opts);
Options process_options(const args_t& arguments);

#endif // OPTIONS_H
