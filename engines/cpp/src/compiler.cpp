#include "compiler.hpp"
#include "args-parser.hpp"

#include <boost/filesystem.hpp>

Compiler::Compiler(Options &options) {
  // validate(options);
  
}

bool Compiler::validate_options(void) {
  
  return false;
}

CompilerResult Compiler::run(void) {
  if (!validate_options()) {
    return INVALID_OPTIONS;
  }

  return COMPILATION_SUCCESS;
}