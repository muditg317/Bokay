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
  std::string compileOut = engineCompile(options, "", testName);
  std::cout << "compiled! -- \n" << compileOut << std::endl;
  std::string runOut = runProgram(options, "", testName, "");
  std::cout << "run! -- \n" << runOut << std::endl;
  return ProgramOutput{compileOut, runOut};
}

void updateTest(std::string testName, bool updateCompilation, bool updateExecution) {
  boost::filesystem::path expectedOutputPath(testOutputFile(options, testName));
  bool hasExistingOutput = boost::filesystem::exists(expectedOutputPath);
  ProgramOutput existingOutput("","");
  if (hasExistingOutput) {
    existingOutput = ProgramOutput(expectedOutputPath);
  }
  ProgramOutput testOutput = runEngineOn(testName);
  std::cout << "write test output to file: " << expectedOutputPath << std::endl;
  if (hasExistingOutput && !updateCompilation) {
    testOutput.setCompilationOutput(existingOutput.getCompilationOutput());
  }
  if (hasExistingOutput && !updateExecution) {
    testOutput.setExecutionOutput(existingOutput.getExecutionOutput());
  }
  testOutput.writeOutputFile(expectedOutputPath);
}


#define ENGINE_TEST(testCase)                                                           \
TEST_CASE(#testCase ".bokay", "[main]") {                                               \
  ProgramOutput actual = runEngineOn(#testCase);                                        \
  boost::filesystem::path expectedOutputPath(testOutputFile(options, #testCase));       \
  std::cout << "checking expected output from: " << expectedOutputPath << std::endl;    \
  REQUIRE(boost::filesystem::exists(expectedOutputPath));                               \
  std::cout << "Expected output exists! " << std::endl;                                 \
  ProgramOutput expected(expectedOutputPath);                                           \
  REQUIRE_THAT(actual.getCompilationOutput(), Catch::Matchers::Equals(expected.getCompilationOutput(), Catch::CaseSensitive::Yes)); \
  REQUIRE_THAT(actual.getExecutionOutput(), Catch::Matchers::Equals(expected.getExecutionOutput(), Catch::CaseSensitive::Yes)); \
}

#define TEST_AS_STRING_WITH_COMMA(testCase) #testCase,

#define ALL_TESTS     \
TEST_MACRO(compiles) 

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
        ["--update"]
        ("which test (or all) should be updated?")
    | Catch::clara::Opt( options.updateCompilation, "updateCompilation" )
        ["--update-comp"]
        ("which test (or all) should have compilation updated?")
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
      bool updateExec = options.updateAll || options.updateAllExecution || options.updateExecution.find(testName) != std::string::npos;
      bool doUpdate = updateComp || updateExec;
      if (options.updateAll || options.update.find(testName) != std::string::npos) {
        updateComp = true;
        updateExec = true;
        doUpdate = true;
      }
      if (doUpdate) {
        std::cout << "Update test: " << testName << (updateComp ? "\tcompilation" : "") << (updateExec ? "\texecution" : "") << std::endl;
        updateTest(testName, updateComp, updateExec);
      } else {
        std::cout << "Don't update test: " << testName << std::endl;
      }
    }
    return 0;
  }

  std::cout << "Running test cases: ";
  for(const auto& testName : allCases) {
    std::cout << testName << ", ";
  }
  std::cout << std::endl;

  return session.run();
}