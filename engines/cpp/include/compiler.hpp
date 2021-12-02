#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "args-parser.hpp"

#include <string>

// #include <boost/filesystem/path.hpp>
// #include <boost/filesystem.hpp>

enum CompilerResult : size_t {
  COMPILATION_SUCCESS,
  INVALID_OPTIONS,
};

class Compiler {
  public:
    Compiler(Options &options);
    CompilerResult run(void);
  private:
    bool validate_options(void);
    // boost::filesystem::path sourceFile;
    // boost::filesystem::path tempFileDir;
    // boost::filesystem::path outputPath;
};

#endif