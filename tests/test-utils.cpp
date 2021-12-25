#include "test-utils.hpp"

#include <cstdio>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <fstream>

#include <fmt/format.h>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

std::vector<std::string> splitByDelim(std::string strToSplit, std::string delim) {
  size_t pos_start = 0, pos_end, delim_len = delim.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = strToSplit.find(delim, pos_start)) != std::string::npos) {
    token = strToSplit.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(strToSplit.substr(pos_start));
  return res;
}

ShellCommandOutput execShellCommand(std::string cmd) {
    std::array<char, 128> buffer;
    std::string stdOut;

    boost::filesystem::path tmpStdErrPath{"./stderr.txt"};
    std::string pipedCmd = fmt::format("{} 2> {}", cmd, tmpStdErrPath.c_str());

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(pipedCmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        stdOut += buffer.data();
    }
    std::string stdErr = readFile(tmpStdErrPath);
    boost::filesystem::remove(tmpStdErrPath);
    return ShellCommandOutput{stdOut, stdErr};
}

std::string readFile(boost::filesystem::path filePath) {
  assert(boost::filesystem::exists(filePath) && "File must exist to be read! Check before calling this method!");
  boost::filesystem::fstream fileStream(filePath);
  std::stringstream buffer;
  buffer << fileStream.rdbuf();
  return buffer.str();
}

std::string engineCompile(TestingOptions options, std::string engineArgs, std::string fileToCompile) {
  // std::cout << "compile file: " << fmt::format("{}/{}.bokay", options.testDir, fileToCompile) << std::endl;
  std::string compilationCommand = fmt::format(
    "{} {} {}/{}.bokay -t {} -o {}/{}",
    options.enginePath, engineArgs, // "{} {}"
    options.testDir, fileToCompile, // "{}/{}.bokay"
    options.buildDir,               // "-t {}"
    options.buildDir, fileToCompile // "-o {}/{}"
  );
  // std::cout << "compile command: " << compilationCommand << std::endl;
  ShellCommandOutput compilationOutput = execShellCommand(compilationCommand);
  // std::cout << "compilation output: " << compilationOutput << std::endl;
  return compilationOutput.toString();
}

std::string runProgram(TestingOptions options, std::string engineArgs, std::string fileToRun, std::string runArgs) {
  boost::filesystem::path programPath(builtTestFile(options, fileToRun));
  if (!boost::filesystem::exists(programPath)) {
    return NOT_FOUND_TEXT("executable");
  }
  std::string executionCommand = fmt::format(
    "{} {}",
    programPath.c_str(),
    runArgs);
  // std::cout << "execute command: " << executionCommand << std::endl;
  ShellCommandOutput executionOutput = execShellCommand(executionCommand);
  // std::cout << "execute output: " << executionOutput << std::endl;
  return executionOutput.toString();
}

std::string ShellCommandOutput::toString(void) {
  return fmt::format("--STDOUT:\n{}\n--STDOUT-END\n--" UNIQUE_TEXT_SECRET "--\n--STDERR:\n{}\n--STDERR-END", stdOut, stdErr);
}

ProgramOutput::ProgramOutput(boost::filesystem::path testOutputFilePath) : ProgramOutput::ProgramOutput() {
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

  auto allOutputs = splitByDelim(out, OUTPUT_DELIM);
  assert(allOutputs.size() == COUNT_OUTPUT_TYPES && "Output file does not have correct number of output sections!");
  for (uint8_t i = 0; i < COUNT_OUTPUT_TYPES; i++) {
    outputs.insert(std::pair<OutputType, std::string>(static_cast<OutputType>(i), allOutputs.at(i)));
  }
}


std::string ProgramOutput::getOutput(OutputType type) {
  return outputs[type];
}

void ProgramOutput::setOutput(OutputType type, std::string newOutput) {
  outputs[type] = newOutput;
}

void ProgramOutput::setFromFile(OutputType type, std::string outputPathString) {
  boost::filesystem::path outputPath(outputPathString);
  if (!boost::filesystem::exists(outputPath)) {
    outputs[type] = fmt::format("{}({}) {}", outputFileNames[type], outputPathString, NOT_FOUND_TEXT(""));
    return;
  }
  outputs[type] = readFile(outputPath);
}

void ProgramOutput::writeOutputFile(boost::filesystem::path testOutputFilePath) {
  constexpr auto read_size = std::size_t{4096};
  std::ofstream stream(testOutputFilePath.c_str());
  stream.exceptions(std::ios_base::badbit);

  std::string mainOutput = fmt::format("{}{}{}", getOutput(COMPILATION_CONSOLE_OUT), OUTPUT_DELIM, getOutput(EXECUTION_OUT));
  std::cout << "Write to file ==============================\n" << mainOutput << "\n===========================" << std::endl;

  // stream << fullOutput;
  for (uint8_t i = 0; i < COUNT_OUTPUT_TYPES; i++) {
    stream << getOutput(static_cast<OutputType>(i));
    if (i < COUNT_OUTPUT_TYPES - 1) {
      stream << OUTPUT_DELIM;
    }
  }
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
  if(update.compare("") || updateCompilation.compare("") || updateIntermediates.compare("") || updateExecution.compare("")) {
    anyUpdates = true;
    if(!updateCompilation.compare("all")) {
      std::cout << "update compilation for all test cases!" << std::endl;
      updateAllCompilation = true;
    }
    if(!updateIntermediates.compare("all")) {
      std::cout << "update intermediates for all test cases!" << std::endl;
      updateAllIntermediates = true;
    }
    if(!updateExecution.compare("all")) {
      std::cout << "update execution for all test cases!" << std::endl;
      updateAllExecution = true;
    }
    if(!update.compare("all") || (updateAllCompilation && updateAllIntermediates && updateAllExecution)) {
      std::cout << "update all test cases!" << std::endl;
      updateAll = true;
      updateAllCompilation = true;
      updateAllIntermediates = true;
      updateAllExecution = true;
    }
  }
  return true;
}