#include "compiler.hpp"
#include "args-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast-builder.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/range/algorithm/count.hpp>
#include <fmt/format.h>
#include <glog/logging.h>

static std::string readFile(boost::filesystem::path filePath) {
  assert(boost::filesystem::exists(filePath) && "File must exist to be read! Check before calling this method!");
  std::fstream fileStream(filePath.string(), std::ios::in);
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  return buffer.str();
}

Compiler::Compiler(Options &options) :
    sourceFile(options.sourceFile),
    outputTemps(options.outputTemps)
{
  sourceName = sourceFile.stem().string();
  // validate(options);
  if (options.outputTemps) {
    tempFileDir = boost::filesystem::path(options.tempFileDir);
  }
  if (options.customOutputPath) {
    outputPath = boost::filesystem::path(options.outputPath);
  } else { // create default output path
    outputPath = boost::filesystem::path(sourceFile);
    // if (outputPath.has_extension()) {
    outputPath.replace_extension(boost::filesystem::path("out"));
    LOG(INFO) << "Using default output path... " << outputPath ;
    // } else {
    //   outputPath.extension
    // }
  }
}

bool Compiler::validate_options(void) {
  if (!boost::filesystem::exists(sourceFile)) { // doesn't exist
    LOG(ERROR) << "Source file (" << sourceFile << ") does not exist!" ;
    return false;
  }
  if (!tempFileDir.empty() && !boost::filesystem::exists(tempFileDir)) { // doesn't exist
    LOG(WARNING) << "Directory for temp files doesn't exist! Creating..." ;
    bool created = boost::filesystem::create_directories(tempFileDir);
    if (!created) return false;
    LOG(INFO) << tempFileDir << " created!" ;
  } else { // temp dir exist
    if (!boost::filesystem::is_directory(tempFileDir)) { // exists but not a dir
      LOG(ERROR) << tempFileDir << " already exists as a file! Exiting..." ;
      return false;
    }
  }
  auto outputParent = outputPath.parent_path();
  if (!boost::filesystem::exists(outputParent)) { // parent of outputPath needs to be created
    LOG(WARNING) << "Creating parent directory for output executable..." ;
    bool created = boost::filesystem::create_directories(outputParent);
    if (!created) return false;
    LOG(INFO) << "\tDone: " << outputParent << " created!" ;
    return false;
  }
  return true;
}

template<class Stage, CompilerResult FailureCode>
typename Stage::Base::OutputType &Compiler::runStage(
    Stage &stage,
    typename Stage::Base::InputType &input
    #ifdef DEBUG
    , std::function<void(typename Stage::Base::OutputType &)> debugCallback
    #endif
  ) const {
  typename Stage::Base::OutputType *output = nullptr;
  typename Stage::Base::ErrorType errorCode = stage(input, output);
  if (errorCode != Stage::Base::SUCCESS_CODE) {
    LOG(ERROR) << "Stage " << Stage::Base::name() << " failed with error code " << static_cast<int>(errorCode) ;
    throw FailureCode;
  }
  #ifdef DEBUG
  debugCallback(*output);
  #endif

  if (outputTemps) {
    boost::filesystem::path tempFile = tempFileDir / fmt::format("{}.{}", sourceName, Stage::Base::tmpOutExt());
    LOG(INFO) << "Writing " << Stage::Base::name() << " outputs to temp file: " << tempFile;
    stage.writeOutput(*output, tempFile);
  }

  return *output;
}

CompilerResult Compiler::run(void) {
  if (!validate_options()) {
    return CompilerResult::INVALID_COMPILATION_OPTIONS;
  }

  fileContents = readFile(sourceFile);

  int lines = boost::range::count(fileContents, '\n') + 1;
  DLOG(INFO) << "File contains " << lines << " lines." ;
  int ind = fileContents.find('\n');
  DLOG(INFO) << "Line 1: " << fileContents.substr(0, ind != std::string::npos ? ind : fileContents.size()) ;

  
  try {
    std::vector<Token> tokens = runStage<Lexer, CompilerResult::FAILED_LEXING>(lexer, fileContents
    #ifdef DEBUG
    , [](std::vector<Token> &tokens) {
      DLOG(INFO) << "Found " << tokens.size() << " tokens!" ;
      std::for_each(tokens.begin(), tokens.end(), [](Token tok) {
        DLOG(INFO) << "Found token: {" << tok.getType() << "}: `" << tok.getContents() << "`" ;
      });
    }
    #endif
    );
  } catch (CompilerResult e) {
    return e;
  }

  std::vector<Token> *tokensPtr = nullptr;
  LexerResult lexerResult = lexer(fileContents, tokensPtr);
  if (lexerResult != LexerResult::LEXING_SUCCESS) {
    LOG(ERROR) << "Lexing has failed! Code: " << static_cast<int>(lexerResult);
    return CompilerResult::FAILED_LEXING;
  }

  DLOG(INFO) << "Found " << tokensPtr->size() << " tokens!" ;

  if (outputTemps) {
    boost::filesystem::path tokenFile = tempFileDir / fmt::format("{}.tok", sourceName);
    LOG(INFO) << "Writing tokens to temp file: " << tokenFile;
    lexer.writeOutput(*tokensPtr, tokenFile);
  }

  std::for_each(tokensPtr->begin(), tokensPtr->end(), [](Token tok) {
    DLOG(INFO) << "Found token: {" << tok.getType() << "}: `" << tok.getContents() << "`" ;
  });

  ParseTree *ptreePtr = nullptr;
  ParserResult parserResult = parser(*tokensPtr, ptreePtr);
  if (parserResult != ParserResult::PARSING_SUCCESS) {
    LOG(ERROR) << "Parsing has failed! Code: " << static_cast<int>(parserResult);
    return CompilerResult::FAILED_PARSING;
  }

  DLOG(INFO) << "Generated parse tree for tokens";

  if (outputTemps) {
    boost::filesystem::path ptreeFile = tempFileDir / fmt::format("{}.ptree", sourceName);
    LOG(INFO) << "Writing parse tree to temp file: " << ptreeFile;
    parser.writeOutput(*ptreePtr, ptreeFile);
  }

  ASTRootNode *astRoot = nullptr;
  ASTBuilderResult astBuilderResult = astBuilder(*ptreePtr, astRoot);
  if (astBuilderResult != ASTBuilderResult::AST_BUILDING_SUCCESS) {
    LOG(ERROR) << "AST building has failed! Code: " << static_cast<int>(astBuilderResult);
    return CompilerResult::FAILED_AST_BUILDING;
  }

  DLOG(INFO) << "Generated AST from parse tree";

  if (outputTemps) {
    boost::filesystem::path astFile = tempFileDir / fmt::format("{}.ast", sourceName);
    LOG(INFO) << "Writing AST to temp file: " << astFile;
    astBuilder.writeOutput(*astRoot, astFile);
  }

  return CompilerResult::COMPILATION_SUCCESS;
}