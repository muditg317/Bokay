#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <fmt/format.h>
#include <array>

std::string engine = ""; // Some user variable you want to be able to set

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

TEST_CASE("run engine", "[main]") {
  std::string execResult = exec(engine);
  std::string expected = fmt::format("1 args:\t{}\t\n", engine);
  REQUIRE_THAT(execResult, Catch::Matchers::Equals(expected));
}

int main( int argc, char* argv[] ) {
  Catch::Session session; // There must be exactly one instance
  
  
  // Build a new parser on top of Catch's
  // using namespace Catch::clara;
  auto cli 
    = session.cli() // Get Catch's composite command line parser
    | Catch::clara::Opt( engine, "engine" ) // bind variable to a new option, with a hint string
        ["-n"]["--engine"]    // the option names it will respond to
        ("which engine should be used?");        // description string for the help output
        
  // Now pass the new composite back to Catch so it uses that
  session.cli( cli );
  
  // Let Catch (using Clara) parse the command line
  int returnCode = session.applyCommandLine( argc, argv );
  if( returnCode != 0 ) // Indicates a command line error
      return returnCode;

  // if set on the command line then 'height' is now set at this point
  if(!engine.compare("")) {
    std::cout << "engine must be set! " << std::endl;
    return 1;
  } else {
    std::cout << "Testing engine: " << engine << std::endl;
  }

  return session.run();
}