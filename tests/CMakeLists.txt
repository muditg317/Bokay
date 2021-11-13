cmake_minimum_required(VERSION 3.16.0)
project(
    Bokay-Engine-Tester
    VERSION 0.1.0
    DESCRIPTION "Test suite for Bokay Language engines")

########################################## find libs
# Adds Catch2::Catch2
find_package(Catch2 CONFIG REQUIRED)
# Adds fmt::fmt
find_package(fmt CONFIG REQUIRED)
# Adds Boost::filesystem
find_package(Boost REQUIRED COMPONENTS filesystem)
########################################## end find libs


# Tests need to be added as executables first
add_executable(test-engine test-engine.cpp)

########################################## build testing libs
add_library(test-utils test-utils.cpp test-utils.hpp test-cases.hpp)
target_link_libraries(test-utils PRIVATE fmt::fmt Boost::filesystem)


# Should be linked to the main library, as well as the Catch2 testing library
target_link_libraries(test-engine PRIVATE Catch2::Catch2 fmt::fmt Boost::filesystem test-utils)

add_dependencies(test-engine bokay-cpp)

# If you register a test, then ctest and make test will run it.
# You can also run examples and check the output, as well.
add_test(NAME C++-Engine
         COMMAND test-engine --engine $<TARGET_FILE:bokay-cpp>
                             --test-dir ${Bokay-Engine-Tester_SOURCE_DIR})

# set_tests_properties(C++-Engine PROPERTIES FIXTURES_REQUIRED bokay-cpp)