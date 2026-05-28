
#include "options.h"

int main(int argc, char **argv) {
  // Create a vector of strings from the input arguments (ignoring argv[0] which
  // is the program name)
  args_t arguments(argv + 1, argv + argc);

  // Call the function to print the arguments
  bool success_flag = process_options(arguments);
  return success_flag ? EXIT_CODE_SUCCESS : EXIT_CODE_FAILURE;
}
