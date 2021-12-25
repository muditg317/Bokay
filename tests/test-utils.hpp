#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <string>
#include <vector>
#include <map>
#include <fmt/format.h>
#include <boost/filesystem.hpp>

#define TEST_OUTPUT_EXT "bokay.test"
#define UNIQUE_TEXT_SECRET "#@$(^*)&%^*%#$&@$#%$^*&(^%*#&$#%^$*&"
#define OUTPUT_DELIM "\n=================" UNIQUE_TEXT_SECRET "==================\n"
#define NOT_FOUND_TEXT(prefix) prefix " FILE NOT FOUND -- SECRET: " UNIQUE_TEXT_SECRET

class TestingOptions {
  public:
    std::string enginePath;
    std::string testDir;
    std::string testOutputDir;
    std::string buildDir;
    std::string update;
    std::string updateCompilation;
    std::string updateIntermediates;
    std::string updateExecution;
    bool anyUpdates;
    bool updateAll;
    bool updateAllCompilation;
    bool updateAllIntermediates;
    bool updateAllExecution;
    bool validate();
};

#define OUTPUTS_WITH_EXT \
X(LEXED_TOKEN_FILE, "tok", "Lexed tokens") \
X(PARSE_TREE_FILE, "ptree", "Parse tree") \
X(AST_FILE, "ast", "AST") \
X(BLOCKS_CONT_FLOW_FILE, "blk", "Blocks w/ control flow") \

#define ALL_OUTPUT_TYPES \
X(COMPILATION_CONSOLE_OUT, "", "Compilation output") \
OUTPUTS_WITH_EXT \
X(EXECUTION_OUT, "", "Execution output")


#define X(type, extension, name) type,
enum OutputType : size_t {
  ALL_OUTPUT_TYPES
  COUNT_OUTPUT_TYPES
};
#undef X

#define X(type, extension, name) extension,
static std::string outputFileExtensions[COUNT_OUTPUT_TYPES] = {ALL_OUTPUT_TYPES};
#undef X

#define X(type, extension, name) name,
static std::string outputFileNames[COUNT_OUTPUT_TYPES] = {ALL_OUTPUT_TYPES};
#undef X

class ShellCommandOutput {
  public:
    ShellCommandOutput(std::string out, std::string err): stdOut(out), stdErr(err) {};
    std::string toString(void);
  private:
    std::string stdOut;
    std::string stdErr;
};

class ProgramOutput {
  public:
    ProgramOutput(boost::filesystem::path);
    ProgramOutput() {};
    std::string getOutput(OutputType);
    void setOutput(OutputType, std::string);
    void setFromFile(OutputType, std::string);
    void writeOutputFile(boost::filesystem::path);
  private:
    std::map<OutputType, std::string> outputs;
};

std::vector<std::string> splitByDelim(std::string strToSplit);

/**
 * @brief runs the provided string as a shell command
 * 
 * @param cmd the command to run in the shell
 * @return ShellCommandOutput a custom format for the output of the command that includes stdout and stderr
 */
ShellCommandOutput execShellCommand(std::string cmd);

std::string readFile(boost::filesystem::path);

/**
 * @brief runs the compiler engine with the provided options / arguments on the specified file
 * 
 * @param options testing options
 * @param engineArgs compilation options
 * @param fileToCompile the file to compile (usually a test case file)
 * @return std::string the output of the compilation (concatenated stdout/stderr)
 */
std::string engineCompile(TestingOptions options, std::string engineArgs, std::string fileToCompile);

std::string runProgram(TestingOptions options, std::string engineArgs, std::string fileToRun, std::string runArgs);

inline std::string testOutputFile(TestingOptions options, std::string testFileName) {
  return fmt::format("{}/{}.{}", options.testOutputDir, testFileName, TEST_OUTPUT_EXT);
}

inline std::string builtTestFile(TestingOptions options, std::string testFileName) {
  return fmt::format("{}/{}", options.buildDir, testFileName);
}




#endif