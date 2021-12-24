#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "args-parser.hpp"
#include "lexer.hpp"

#include <string>

#include <boost/filesystem/path.hpp>
// #include <boost/filesystem.hpp>

enum class CompilerResult {
  COMPILATION_SUCCESS,
  INVALID_COMPILATION_OPTIONS,
  FAILED_LEXING,
};

class Compiler {
  public:
    Compiler(Options &options);
    CompilerResult run(void);
  private:
    Lexer lexer;
    bool validate_options(void);
    boost::filesystem::path sourceFile;
    boost::filesystem::path tempFileDir;
    boost::filesystem::path outputPath;
    std::string fileContents;
};

#endif