cmake_minimum_required(VERSION 3.16.0)
project(
    Bokay-Engine-Tester
    VERSION 0.1.0
    DESCRIPTION "Test suite for Bokay Language engines")


# Adds Catch2::Catch2
find_package(Catch2 CONFIG REQUIRED)

# Tests need to be added as executables first
add_executable(test-cpp-engine test-cpp-engine.cpp)

# I'm using C++17 in the test
# target_compile_features(test-cpp-engine PRIVATE cxx_std_17)

# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(test-cpp-engine PRIVATE Catch2::Catch2)

add_dependencies(test-cpp-engine bokay-cpp)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME C++-Engine
         COMMAND test-cpp-engine) # Command can be a target
