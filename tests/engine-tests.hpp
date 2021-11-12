#ifndef TESTS_HPP
#define TESTS_HPP

#ifndef ENGINE
#error must specify engine to test before including!
#endif

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

TEST_CASE( "run engine", "[main]" ) {
  std::string execResult = exec(ENGINE);
  std::string expected = "1 args:\t";
  expected += ENGINE;
  expected += "\t\n";
  std::cout << "OUTPUT FROM -- exec(ENGINE) -- \n" << std::endl;
  std::cout << execResult << std::endl;
  std::cout << "\n=========================\nOUTPUT EXPECTED -- \n" << std::endl;
  std::cout << expected << std::endl;
  std::cout << "\n=========================" << std::endl;
  REQUIRE_THAT(execResult, Catch::Matchers::Equals(expected));
}
TEST_CASE( ENGINE, "[main]" ) {
  std::cout << ENGINE << std::endl;
  // REQUIRE(true);
}


#endif