#ifndef ARGS_PARSER_HPP
#define ARGS_PARSER_HPP

#include <string>

// argument labels =====================
#define ARG_HELP "help"
#define ARG_HELP_ALIAS "h"
#define ARG_VERSION "version"
#define ARG_VERSION_ALIAS "v"
#define ARG_CONFIG "config"
#define ARG_CONFIG_ALIAS "c"
#define ARG_TEMP_FILE_DIR "temp-file-dir"
#define ARG_TEMP_FILE_DIR_ALIAS "t"
#define ARG_OUTPUT "output"
#define ARG_OUTPUT_ALIAS "o"
#define ARG_INPUT_FILE "input-file"
#define ARG_INPUT_FILE_ALIAS "i"

#define OPT_W_ALIAS(name) name "," name##_ALIAS
// argument labels =====================

typedef struct {
  std::string sourceFile;
  std::string tempFileDir;
  bool outputTemps;
  std::string outputPath;
  bool customOutputPath;
} Options;

enum class ParseResult {
  PARSING_SUCCESS,
  EXIT_COMPILATION,
  INVALID_ARGUMENTS,
};

ParseResult parseCommandLine(int argc, char *argv[], Options &options);





#endif