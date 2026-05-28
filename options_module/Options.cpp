#include "Options.h"
#include <iostream>
#include <string>

void print_unkown(const std::string& option) {
  std::cout << "Unknown option <" << option << "> found!\n";
}

void read_value_option(const std::string& option, iterator_t& it,
                       iterator_t end, std::string& out, bool& valid) {
  if (++it != end) {
    out = *it;
  } else {
    std::cout << "Option <" << option << "> requires a value.\n";
    valid = false;
  }
}

void process_short_options(const std::string& arg, iterator_t& it,
                           iterator_t end, Options& opts) {
  for (size_t j = 1; j < arg.size(); ++j) {
    switch (arg[j]) {
    case OPTION_VERBOSE_SHORT:
      opts.verbose = true;
      break;
    default:
      print_unkown(arg.substr(j, 1));
      opts.valid = false;
      break;
    }
  }
}

void process_long_options(const std::string& arg, iterator_t& it,
                          iterator_t end, Options& opts) {
  std::string name = arg.substr(2);
  if (name == OPTION_VERBOSE_LONG) {
    opts.verbose = true;
  } else if (name == VALUE_OPTION_RESUME_LONG) {
    read_value_option(name, it, end, opts.resume_file, opts.valid);
  } else if (name == VALUE_OPTION_CHECK_LONG) {
    opts.check_mode = true;
    read_value_option(name, it, end, opts.check_file, opts.valid);
  } else {
    print_unkown(name);
    opts.valid = false;
  }
}

Options process_options(const args_t& arguments) {
  Options opts;
  for (auto it = arguments.begin(); it != arguments.end(); ++it) {
    const std::string& arg = *it;
    if (arg.rfind("--", 0) == 0)
      process_long_options(arg, it, arguments.end(), opts);
    else if (arg[0] == '-')
      process_short_options(arg, it, arguments.end(), opts);
    else {
      std::cout << "Unexpected argument <" << arg << ">\n";
      opts.valid = false;
    }
  }
  return opts;
}
