#pragma once

#include "utilities.hpp"
#include "compiler-stage.hpp"
#include "pipeline.hpp"
#include "args-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast-builder.hpp"

#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

#include <boost/filesystem/path.hpp>
// #include <boost/filesystem.hpp>

enum class CompilerResult {
  COMPILATION_SUCCESS,
  INVALID_COMPILATION_OPTIONS,
  FAILED_LEXING,
  FAILED_PARSING,
  FAILED_AST_BUILDING,
};


#define ALL_STAGES \
X(Lexer, tok, FAILED_LEXING, Parser) X_delim \
X(Parser, ptree, FAILED_PARSING, ASTBuilder) X_delim \
X(ASTBuilder, ast, FAILED_AST_BUILDING, void)

#define X_delim , // must be reset to , after use (if changed)

//====== define various compile-time mappings ======//
#define ALL_MAP_ENTRIES ALL_STAGES

#define X(Stage, ext, compRes, next) MAPPING_ENTRY(TypeToValue<CompilerResult>, Stage, CompilerResult::compRes)
CREATE_MAPPING(constexpr auto, failure_code_for, TypeToValue<CompilerResult>)
#undef X

#define X(Stage, ext, compRes, next) MAPPING_ENTRY(TypeToString, Stage, Stage::Base::TMP_OUT_EXT_)
CREATE_MAPPING(constexpr auto, tmp_file_ext, TypeToString)
#undef X

#define X(Stage, ext, compRes, next) MAPPING_ENTRY(TypeToType, Stage, next)
CREATE_MAPPING(using, next_stage_for, TypeToType)
#undef X

#undef ALL_MAP_ENTRIES
//====== define various compile-time mappings ======//


class Compiler {
 public:
  Compiler(Options &options);
  CompilerResult operator()(void);
 private:
  #define X(Stage, ext, compRes, next) Stage
  Pipeline<ALL_STAGES> pipeline;
  #undef X

  bool validate_options(void);
  bool outputTemps;
  std::string sourceName;
  boost::filesystem::path sourceFile;
  boost::filesystem::path tempFileDir;
  boost::filesystem::path outputPath;
  std::string fileContents;
};