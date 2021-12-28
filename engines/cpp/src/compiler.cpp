#include "compiler.hpp"
#include "args-parser.hpp"
#include "lexer.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/range/algorithm/count.hpp>
#include <fmt/format.h>
#include <glog/logging.h>

static std::string readFile(boost::filesystem::path filePath) {
  assert(boost::filesystem::exists(filePath) && "File must exist to be read! Check before calling this method!");
  boost::filesystem::fstream fileStream(filePath);
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  return buffer.str();
}

Compiler::Compiler(Options &options) :
    sourceFile(options.sourceFile),
    outputTemps(options.outputTemps),
    lexer(options)
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
  if (!boost::filesystem::exists(tempFileDir)) { // doesn't exist
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

CompilerResult Compiler::run(void) {
  if (!validate_options()) {
    return CompilerResult::INVALID_COMPILATION_OPTIONS;
  }

  fileContents = readFile(sourceFile);

  int lines = boost::range::count(fileContents, '\n') + 1;
  DLOG(INFO) << "File contains " << lines << " lines." ;
  int ind = fileContents.find('\n');
  DLOG(INFO) << "Line 1: " << fileContents.substr(0, ind != std::string::npos ? ind : fileContents.size()) ;

  std::vector<Token> tokens;
  LexerResult result = lexer.run(fileContents, tokens);
  if (result != LexerResult::LEXING_SUCCESS) {
    LOG(ERROR) << "Lexing has failed! Code: " << static_cast<int>(result);
    return CompilerResult::FAILED_LEXING;
  }

  DLOG(INFO) << "Found " << tokens.size() << " tokens!" ;

  if (outputTemps) {
    boost::filesystem::path tokenFile = tempFileDir / fmt::format("{}.tok", sourceName);
    LOG(INFO) << "Writing tokens to temp file: " << tokenFile ;
    lexer.writeTokens(tokens, tokenFile);
  }

  std::for_each(tokens.begin(), tokens.end(), [](Token tok) {
    DLOG(INFO) << "Found token: {" << tok.getType() << "}: `" << tok.getContents() << "`" ;
  });

  return CompilerResult::COMPILATION_SUCCESS;
}