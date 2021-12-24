#ifndef ARGS_PARSER_HPP
#define ARGS_PARSER_HPP

#include <string>

// argument labels =====================
#define HELP "help"
#define HELP_ALIAS "h"
#define VERSION "version"
#define VERSION_ALIAS "v"
#define CONFIG "config"
#define CONFIG_ALIAS "c"
#define TEMP_FILE_DIR "temp-file-dir"
#define TEMP_FILE_DIR_ALIAS "t"
#define OUTPUT "output"
#define OUTPUT_ALIAS "o"
#define INPUT_FILE "input-file"
#define INPUT_FILE_ALIAS "i"

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