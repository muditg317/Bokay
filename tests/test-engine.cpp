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

#include "test-cases.hpp"

TestingOptions options;

ProgramOutput runEngineOn(std::string testName) {
  std::string compileOut = engineCompile(options, "", testName);
  std::cout << "compiled! -- \n" << compileOut << std::endl;
  std::string runOut = runProgram(options, "", testName, "");
  std::cout << "run! -- \n" << runOut << std::endl;
  return ProgramOutput{compileOut, runOut};
}

void updateTest(std::string testName) {
  ProgramOutput testOutput = runEngineOn(testName);
  boost::filesystem::path expectedOutputPath(testOutputFile(options, testName));
  std::cout << "write test output to file: " << expectedOutputPath << std::endl;
  testOutput.writeOutputFile(expectedOutputPath);
}

#define ENGINE_TEST(testCase)                                                           \
TEST_CASE(#testCase ".bokay", "[main]") {                                               \
  ProgramOutput actual = runEngineOn(#testCase);                                         \
  boost::filesystem::path expectedOutputPath(testOutputFile(options, #testCase));       \
  std::cout << "checking expected output from: " << expectedOutputPath << std::endl;    \
  REQUIRE(boost::filesystem::exists(expectedOutputPath));                               \
  std::cout << "Expected output exists! " << std::endl;                                 \
  ProgramOutput expected(expectedOutputPath);                                            \
}
ALL_TESTS
#undef ENGINE_TEST

#define ENGINE_TEST(testCase) #testCase,
std::string allCases[] = { ALL_TESTS };
#undef ENGINE_TEST

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
        ("which test (or all) should be updated?");
        
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
      if (options.updateAll || options.update.find(testName) != std::string::npos) {
        std::cout << "Update test: " << testName << std::endl;
        updateTest(testName);
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