#include "compiler.hpp"
#include "args-parser.hpp"

#include <iostream>
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>
#include <boost/range/algorithm/count.hpp>

static std::string readFile(boost::filesystem::path filePath) {
  assert(boost::filesystem::exists(filePath) && "File must exist to be read! Check before calling this method!");
  boost::filesystem::fstream fileStream(filePath);
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  return buffer.str();
}

Compiler::Compiler(Options &options) :
    sourceFile(options.sourceFile)
    // tempFileDir(options.tempFileDir),
    // outputPath(options.outputPath) 
{
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
    std::cout << "Using default output path... " << outputPath << std::endl;
    // } else {
    //   outputPath.extension
    // }
  }
}

bool Compiler::validate_options(void) {
  if (!boost::filesystem::exists(sourceFile)) { // doesn't exist
    std::cout << "Source file (" << sourceFile << ") does not exist!" << std::endl;
    return false;
  }
  if (!boost::filesystem::exists(tempFileDir)) { // doesn't exist
    std::cout << "Directory for temp files doesn't exist! Creating..." << std::endl;
    bool created = boost::filesystem::create_directories(tempFileDir);
    if (!created) return false;
    std::cout << tempFileDir << " created!" << std::endl;
  } else { // temp dir exist
    if (!boost::filesystem::is_directory(tempFileDir)) { // exists but not a dir
      std::cout << tempFileDir << " already exists as a file! Exiting..." << std::endl;
      return false;
    }
  }
  auto outputParent = outputPath.parent_path();
  if (!boost::filesystem::exists(outputParent)) { // parent of outputPath needs to be created
    std::cout << "Creating parent directory for output executable..." << std::endl;
    bool created = boost::filesystem::create_directories(outputParent);
    if (!created) return false;
    std::cout << outputParent << " created!" << std::endl;
    return false;
  }
  return true;
}

CompilerResult Compiler::run(void) {
  if (!validate_options()) {
    return INVALID_OPTIONS;
  }

  fileContents = readFile(sourceFile);

  int lines = boost::range::count(fileContents, '\n') + 1;
  std::cout << "File contains " << lines << " lines." << std::endl;
  int ind = fileContents.find('\n');
  std::cout << "Line 1: " << fileContents.substr(0, ind != std::string::npos ? ind : fileContents.size()) << std::endl;

  


  return COMPILATION_SUCCESS;
}