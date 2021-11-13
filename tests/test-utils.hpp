#ifndef TEST_UTILS_H
#define TEST_UTILS_H
#define CATCH_CONFIG_IMPL_ONLY

#include <string>
#include <vector>
#include <fmt/format.h>
#include <boost/filesystem.hpp>

#define TEST_OUTPUT_EXT "bokay.test"
#define UNIQUE_TEXT_SECRET "#@$(^*)&%^*%#$&@$#%$^*&(^%*#&$#%^$*&"
#define COMPILE_RUN_DELIM "\n=================" UNIQUE_TEXT_SECRET "==================\n"
#define PROGRAM_NOT_FOUND_TEXT "PROGRAM NOT FOUND -- SECRET: " UNIQUE_TEXT_SECRET

class TestingOptions {
  public:
    std::string enginePath;
    std::string testDir;
    std::string testOutputDir;
    std::string buildDir;
    std::string update;
    bool anyUpdates;
    bool updateAll;
    bool validate();
};

class ProgramOutput {
  public:
    ProgramOutput(boost::filesystem::path);
    ProgramOutput(std::string, std::string);
    std::string getCompilationOutput();
    std::string getExecutionOutput();
    void writeOutputFile(boost::filesystem::path);
  private:
    std::string compilationOutput;
    std::string executionOutput;
};

std::vector<std::string> splitByDelim(std::string strToSplit);

std::string exec(std::string);

enum EngineCommand { RUN,COMPILE };
extern const char *EngineCommands[];

std::string engineCompile(TestingOptions options, std::string engineArgs, std::string fileToCompile);

std::string runProgram(TestingOptions options, std::string engineArgs, std::string fileToRun, std::string runArgs);

inline std::string testOutputFile(TestingOptions options, std::string testFileName) {
  return fmt::format("{}/{}.{}", options.testOutputDir, testFileName, TEST_OUTPUT_EXT);
}

inline std::string builtTestFile(TestingOptions options, std::string testFileName) {
  return fmt::format("{}/{}", options.buildDir, testFileName);
}




#endif