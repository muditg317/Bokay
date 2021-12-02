#include "args-parser.hpp"
#include "compiler.hpp"

#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {

  Options options;
  ParseResult ret = parseCommandLine(argc, argv, options);
  if (ret != PARSING_SUCCESS) {
    return ret;
  }

  Compiler compiler(options);

  try {
    CompilerResult result = compiler.run();
    // if (result != COMPILATION_SUCCESS) {
    return result;
    // }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}