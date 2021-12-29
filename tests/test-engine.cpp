#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <fmt/format.h>
#include <array>
#include <fstream>

#include <boost/filesystem.hpp>

#include "test-utils.hpp"

TestingOptions options;

ProgramOutput runEngineOn(std::string testName) {
  ProgramOutput output;

  std::string compileOut = engineCompile(options, "", testName);
  // std::cout << "compiled! -- \n" << compileOut << std::endl;
  output.setOutput(COMPILATION_CONSOLE_OUT, compileOut);

  #define X(type, extension, name) output.setFromFile(type, fmt::format("{}/{}.{}", options.buildDir, testName, extension));
  OUTPUTS_WITH_EXT
  #undef X

  std::string executionOut = runProgram(options, "", testName, "");
  // std::cout << "run! -- \n" << executionOut << std::endl;
  output.setOutput(EXECUTION_OUT, executionOut);

  return output;
}

void updateTest(std::string testName, bool updateCompilation, bool updateIntermediates, bool updateExecution) {
  boost::filesystem::path expectedOutputPath(testOutputFile(options, testName));
  bool hasExistingOutput = boost::filesystem::exists(expectedOutputPath);
  ProgramOutput existingOutput;
  if (hasExistingOutput && !(updateCompilation && updateIntermediates && updateExecution)) {
    existingOutput = ProgramOutput(expectedOutputPath);
  }
  ProgramOutput testOutput = runEngineOn(testName);
  std::cout << "write test output to file: " << expectedOutputPath << std::endl;
  if (hasExistingOutput && !updateCompilation) {
    testOutput.setOutput(COMPILATION_CONSOLE_OUT, existingOutput.getOutput(COMPILATION_CONSOLE_OUT));
  }
  if (hasExistingOutput && !updateIntermediates) {
    static_assert(COMPILATION_CONSOLE_OUT == 0, "Compilation output must be the 0th output!");
    static_assert(EXECUTION_OUT == (COUNT_OUTPUT_TYPES-1), "Execution output must be the last output!");
    for (uint8_t i = COMPILATION_CONSOLE_OUT+1; i < EXECUTION_OUT; i++) {
      testOutput.setOutput(static_cast<OutputType>(i), existingOutput.getOutput(static_cast<OutputType>(i)));
    }
  }
  if (hasExistingOutput && !updateExecution) {
    testOutput.setOutput(EXECUTION_OUT, existingOutput.getOutput(EXECUTION_OUT));
  }
  testOutput.writeOutputFile(expectedOutputPath);
}

/* use this to fail all tests
REQUIRE(false);}\
  //
*/

#define ENGINE_TEST(testCase)                                                           \
TEST_CASE(#testCase ".bokay", "[main]") {                                               \
  ProgramOutput actual = runEngineOn(#testCase);                                        \
  boost::filesystem::path expectedOutputPath(testOutputFile(options, #testCase));       \
  std::cout << "checking expected output from: " << expectedOutputPath << std::endl;    \
  REQUIRE(boost::filesystem::exists(expectedOutputPath));                               \
  std::cout << "Expected output exists! " << std::endl;                                 \
  ProgramOutput expected(expectedOutputPath);                                           \
  for (uint8_t i = 0; i < COUNT_OUTPUT_TYPES; i++) {                                        \
    REQUIRE_THAT(actual.getOutput(static_cast<OutputType>(i)),                          \
        Catch::Matchers::Equals(                                                        \
            expected.getOutput(static_cast<OutputType>(i)),                             \
            Catch::CaseSensitive::Yes));                                                \
  }                                                                                     \
}

#define TEST_AS_STRING_WITH_COMMA(testCase) #testCase,

#define ALL_TESTS     \
TEST_MACRO(one-comment) \
TEST_MACRO(basic-types) \
// TEST_MACRO(full-design) \


#define TEST_MACRO(testCase) ENGINE_TEST(testCase)
ALL_TESTS
#undef TEST_MACRO

#define TEST_MACRO(testCase) TEST_AS_STRING_WITH_COMMA(testCase)
std::string allCases[] = { ALL_TESTS };
#undef TEST_MACRO

int main( int argc, char* argv[] ) {
  Catch::Session session; // There must be exactly one instance
  
  // Build a new parser on top of Catch's
  // available options: ghjkmnpquyz 
  auto cli 
    = session.cli() // Get Catch's composite command line parser
    | Catch::clara::Opt( options.enginePath, "engine" )
        ["--engine"]
        ("which engine should be used?")
    | Catch::clara::Opt( options.testDir, "testDir" )
        ["--test-dir"]
        ("where are the tests saved?")
    | Catch::clara::Opt( options.update, "update" )
        ["-u"]["--update"]
        ("which test (or all) should be updated?")
    | Catch::clara::Opt( options.updateCompilation, "updateCompilation" )
        ["--update-comp"]
        ("which test (or all) should have compilation updated?")
    | Catch::clara::Opt( options.updateIntermediates, "updateIntermediates" )
        ["--update-intm"]
        ("which test (or all) should have intermediates updated?")
    | Catch::clara::Opt( options.updateExecution, "updateExecution" )
        ["--update-exec"]
        ("which test (or all) should have execution updated?");
        
  // Now pass the new composite back to Catch so it uses that
  session.cli( cli );
  
  // Let Catch (using Clara) parse the command line
  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
      return returnCode;

  // ensure that engine and testDir are configured
  if ( !options.validate() ) {
    return 1;
  }

  if (options.anyUpdates) {
    std::cout << "Run updates! " << options.update << std::endl;
    for(const auto& testName : allCases) {
      bool updateComp = options.updateAll || options.updateAllCompilation || options.updateCompilation.find(testName) != std::string::npos;
      bool updateIntm = options.updateAll || options.updateAllIntermediates || options.updateIntermediates.find(testName) != std::string::npos;
      bool updateExec = options.updateAll || options.updateAllExecution || options.updateExecution.find(testName) != std::string::npos;
      bool doUpdate = updateComp || updateIntm || updateExec;
      if (options.updateAll || options.update.find(testName) != std::string::npos) {
        updateComp = true;
        updateIntm = true;
        updateExec = true;
        doUpdate = true;
      }
      if (doUpdate) {
        std::cout << "Update test: " << testName << " -- " << (updateComp ? "\tcompilation" : "") << (updateIntm ? "\tintermediates" : "") << (updateExec ? "\texecution" : "") << std::endl;
        updateTest(testName, updateComp, updateIntm, updateExec);
      } else {
        std::cout << "Don't update test: " << testName << std::endl;
      }
    }
    return 0;
  }

  std::cout << "Running test cases: ";
  for(const auto& testName : allCases) {
    std::cout << testName << "\t,\t";
  }
  std::cout << std::endl;

  return session.run();
}