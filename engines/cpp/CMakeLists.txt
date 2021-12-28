cmake_minimum_required(VERSION 3.16.0)
project(
    Bokay-Engine-C++
    VERSION 0.1.0
    DESCRIPTION "A C++ compiling engine (lexer,parser,compiler) for the Bokay language")

##################################################
# include Boost library (useful for args parsing)
# set(Boost_USE_STATIC_LIBS OFF)
# set(Boost_USE_MULTITHREADED ON)
# set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost QUIET REQUIRED COMPONENTS program_options filesystem) # algorithm is header-only
message(STATUS "Boost::boost version: ${Boost_VERSION}")

# Adds fmt::fmt
find_package(fmt CONFIG REQUIRED)
message(STATUS "fmt::fmt version: ${fmt_VERSION}")

# Adds glog::glog
find_package(glog CONFIG REQUIRED)
message(STATUS "glog::glog version: ${glog_VERSION}")

# This is needed if your Boost version is newer than your CMake version
# or if you have an old version of CMake (<3.5)
# if(NOT TARGET Boost::program_options)
#     add_library(Boost::program_options IMPORTED INTERFACE)
#     set_property(TARGET Boost::program_options PROPERTY
#         INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})
#     set_property(TARGET Boost::program_options PROPERTY
#         INTERFACE_LINK_LIBRARIES ${Boost_LIBRARIES})
# endif()
# End Boost setup
##################################################

set(Bokay-Engine-C++_INCLUDE_DIR "${Bokay-Engine-C++_SOURCE_DIR}/include")

# file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Bokay-Engine-C++_INCLUDE_DIR}/*.hpp")
# message(STATUS ${HEADER_LIST})

add_library(LibArgsParser src/args-parser.cpp include/args-parser.hpp)
add_library(LibCompiler src/compiler.cpp include/compiler.hpp)
add_library(LibLexer src/lexer.cpp include/lexer.hpp)

# We need this directory, and users of our library will not need it too
target_include_directories(LibArgsParser PRIVATE ${Bokay-Engine-C++_INCLUDE_DIR})
target_include_directories(LibCompiler PRIVATE ${Bokay-Engine-C++_INCLUDE_DIR})
target_include_directories(LibLexer PRIVATE ${Bokay-Engine-C++_INCLUDE_DIR})

target_link_libraries(LibArgsParser PRIVATE Boost::program_options glog::glog)
target_link_libraries(LibCompiler PRIVATE Boost::filesystem fmt::fmt glog::glog)
target_link_libraries(LibLexer PRIVATE Boost::headers Boost::filesystem fmt::fmt glog::glog)


add_executable(bokay-cpp main.cpp)
set_clang_tidy(bokay-cpp)
# target_link_options(bokay-cpp PRIVATE "-v")

target_include_directories(bokay-cpp PRIVATE ${Bokay-Engine-C++_INCLUDE_DIR})
target_link_libraries(bokay-cpp PRIVATE
                                        Boost::boost
                                        glog::glog
                                        LibArgsParser
                                        LibCompiler
                                        LibLexer)

add_custom_command(TARGET bokay-cpp
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:bokay-cpp> ${Bokay_SOURCE_DIR})