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
find_package(Boost QUIET REQUIRED COMPONENTS program_options)
message(STATUS "Boost::program_options version: ${Boost_VERSION}")

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

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Bokay-Engine-C++_INCLUDE_DIR}/*.hpp")

message(STATUS ${HEADER_LIST})

add_library(LibBokayFile src/bokay-file.cpp include/bokay-file.hpp)

# We need this directory, and users of our library will need it too
target_include_directories(LibBokayFile PUBLIC ${Bokay-Engine-C++_INCLUDE_DIR})


add_executable(bokay-cpp src/main.cpp)
set_clang_tidy(bokay-cpp)

target_link_libraries(bokay-cpp PRIVATE Boost::program_options LibBokayFile)

