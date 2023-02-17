#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "args-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast-builder.hpp"

#include <string>

#include <boost/filesystem/path.hpp>
// #include <boost/filesystem.hpp>

enum class CompilerResult {
  COMPILATION_SUCCESS,
  INVALID_COMPILATION_OPTIONS,
  FAILED_LEXING,
  FAILED_PARSING,
  FAILED_AST_BUILDING,
};

class Compiler {
  public:
    Compiler(Options &options);
    CompilerResult run(void);
  private:
    Lexer lexer;
    Parser parser;
    ASTBuilder astBuilder;
    bool validate_options(void);
    bool outputTemps;
    std::string sourceName;
    boost::filesystem::path sourceFile;
    boost::filesystem::path tempFileDir;
    boost::filesystem::path outputPath;
    std::string fileContents;
};

#endif