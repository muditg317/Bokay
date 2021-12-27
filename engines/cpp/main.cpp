#include "args-parser.hpp"
#include "compiler.hpp"

#include <exception>
#include <iostream>
// #define TESTING
#ifdef TESTING
#include <regex>
#define REGEX "(\\n)"
int main(int argc, char *argv[]) {
  std::string comment = "\n\n";
  std::cout << "the string: `" << comment << "`\n";
  std::regex re{REGEX};
  std::cout << "the regex: `" REGEX "`\n";
  std::smatch match_result;
  bool hasMatch = std::regex_search(comment.cbegin(), comment.cend(), match_result, re);
  std::cout << "hasMatch: " << hasMatch << std::endl;
  std::cout << "size: " << match_result.size() << std::endl;
  std::cout << "length: " << match_result.length() << std::endl;
}
#else
int main(int argc, char *argv[]) {

  Options options;
  ParseResult ret = parseCommandLine(argc, argv, options);
  if (ret != ParseResult::PARSING_SUCCESS) {
    return static_cast<int>(ret);
  }

  Compiler compiler(options);

  try {
    CompilerResult result = compiler.run();
    // if (result != COMPILATION_SUCCESS) {
    return static_cast<int>(result);
    // }
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return 1;
  }
}
#endif