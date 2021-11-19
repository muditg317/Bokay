#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <fmt/format.h>
#include <array>
#include <fstream>
#include "test-utils.hpp"

std::vector<std::string> splitByDelim(std::string strToSplit, std::string delim) {
  size_t pos_start = 0, pos_end, delim_len = delim.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = strToSplit.find (delim, pos_start)) != std::string::npos) {
    token = strToSplit.substr (pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back (token);
  }

  res.push_back (strToSplit.substr (pos_start));
  return res;
}

std::string exec(std::string cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

const char *EngineCommands[] = {"run","compile"};

std::string engineCompile(TestingOptions options, std::string engineArgs, std::string fileToCompile) {
  // std::cout << "compile file: " << fmt::format("{}/{}.bokay", options.testDir, fileToCompile) << std::endl;
  std::string compilationCommand = fmt::format(
    "{} {} {} {}/{}.bokay -o {}/{}",
    options.enginePath, EngineCommands[COMPILE], engineArgs,
    options.testDir, fileToCompile,
    options.buildDir, fileToCompile
  );
  // std::cout << "compile command: " << compilationCommand << std::endl;
  std::string compilationOutput = exec(compilationCommand);
  // std::cout << "compilation output: " << compilationOutput << std::endl;
  return compilationOutput;
}

std::string runProgram(TestingOptions options, std::string engineArgs, std::string fileToRun, std::string runArgs) {
  boost::filesystem::path programPath(builtTestFile(options, fileToRun));
  if (!boost::filesystem::exists(programPath)) {
    return PROGRAM_NOT_FOUND_TEXT;
  }
  std::string executionCommand = fmt::format(
    "{} {}",
    programPath.c_str(),
    runArgs);
  // std::cout << "execute command: " << executionCommand << std::endl;
  std::string executionOutput = exec(executionCommand);
  // std::cout << "execute output: " << executionOutput << std::endl;
  return executionOutput;
}

ProgramOutput::ProgramOutput(boost::filesystem::path testOutputFilePath) {
  constexpr auto read_size = std::size_t{4096};
  std::ifstream stream(testOutputFilePath.c_str());
  stream.exceptions(std::ios_base::badbit);

  auto out = std::string{};
  auto buf = std::string(read_size, '\0');
  while (stream.read(& buf[0], read_size)) {
      out.append(buf, 0, stream.gcount());
  }
  out.append(buf, 0, stream.gcount());
  stream.close();

  auto bothOutputs = splitByDelim(out, COMPILE_RUN_DELIM);
  compilationOutput = bothOutputs.at(0);
  executionOutput = bothOutputs.at(1);
}

ProgramOutput::ProgramOutput(std::string _compilationOutput, std::string _executionOutput) {
  compilationOutput = _compilationOutput;
  executionOutput = _executionOutput;
}

std::string ProgramOutput::getCompilationOutput() {
  return compilationOutput;
}

std::string ProgramOutput::getExecutionOutput() {
  return executionOutput;
}

void ProgramOutput::setCompilationOutput(std::string newOutput) {
  compilationOutput = newOutput;
}

void ProgramOutput::setExecutionOutput(std::string newOutput) {
  executionOutput = newOutput;
}

void ProgramOutput::writeOutputFile(boost::filesystem::path testOutputFilePath) {
  constexpr auto read_size = std::size_t{4096};
  std::ofstream stream(testOutputFilePath.c_str());
  stream.exceptions(std::ios_base::badbit);

  std::string fullOutput = fmt::format("{}{}{}", compilationOutput, COMPILE_RUN_DELIM, executionOutput);

  std::cout << "Write to file ==============================\n" << fullOutput << "\n===========================" << std::endl;

  stream << fullOutput;
  stream.close();
}

bool TestingOptions::validate() {
  if(!enginePath.compare("")) {
    std::cout << "engine must be set! " << std::endl;
    return false;
  } else {
    std::cout << "Testing engine: " << enginePath << std::endl;
  }
  if(!testDir.compare("")) {
    std::cout << "testDir must be set! " << std::endl;
    return false;
  } else {
    std::cout << "Test directory: " << testDir << std::endl;
  }
  testOutputDir = testDir;
  buildDir = fmt::format("{}/build", testDir);
  if(update.compare("") || updateCompilation.compare("") || updateExecution.compare("")) {
    anyUpdates = true;
    if(!updateCompilation.compare("all")) {
      std::cout << "update compilation for all test cases!" << std::endl;
      updateAllCompilation = true;
    }
    if(!updateExecution.compare("all")) {
      std::cout << "update execution for all test cases!" << std::endl;
      updateAllExecution = true;
    }
    if(!update.compare("all") || (updateAllCompilation && updateAllExecution)) {
      std::cout << "update all test cases!" << std::endl;
      updateAll = true;
      updateAllCompilation = true;
      updateAllExecution = true;
    }
  }
  return true;
}