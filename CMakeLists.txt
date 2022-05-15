cmake_minimum_required(VERSION 3.16.0)
project(Bokay   VERSION 0.1.0
                DESCRIPTION "A custom language built from scratch"
                LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

### Configure testing
set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
include(CTest)
enable_testing()


##################################################
# Configure clang-tidy linting
unset(CLANG_TIDY_EXECUTABLE)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy")
  find_program(CLANG_TIDY_EXECUTABLE "clang-tidy" DOC "Path to clang-tidy executable")
  if(CLANG_TIDY_EXECUTABLE MATCHES "CLANG_TIDY_EXECUTABLE")
    unset(CLANG_TIDY_EXECUTABLE)
    message(WARNING "   WARNING: .clang-tidy file found for project ${PROJECT_NAME}, yet clang-tidy not on PATH so disabling lint pass")
  endif()
endif()

# function to add clang-tidy linting to a target
function(SET_CLANG_TIDY TARGET_FOR_CLANG_TIDY)
    message(STATUS "Configuring clang-tidy for: ${TARGET_FOR_CLANG_TIDY}")
    if(DEFINED CLANG_TIDY_EXECUTABLE)
        if(MSVC)
            # Tell clang-tidy to interpret these parameters as clang-cl would
            set_target_properties(${TARGET_FOR_CLANG_TIDY} PROPERTIES
                CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE};-fms-extensions;-fms-compatibility-version=19;-D_M_AMD64=100;
            )
        else()
            set_target_properties(${TARGET_FOR_CLANG_TIDY} PROPERTIES
                CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE}
            )
        endif()
        message(STATUS "    clang-tidy set")
    endif()
endfunction()
# End clang-tidy setup
##################################################


##################################################
# Add various inclusions `include([lib])`
# FetchContent added in CMake 3.11, downloads during the configure step
# include(FetchContent)
# End inclusions
##################################################

##################################################
# Add packages `find_package([package])`
# list(APPEND CMAKE_PREFIX_PATH "/home/muditg317/application-installs/vcpkg/installed/x64-linux/share")
# End packages
##################################################

# add_custom_target(clean COMMAND rm -rf "${Bokay_SOURCE_DIR}/build")

add_subdirectory(engines)   # look in hello subdirectory for CMakeLists.txt to process


# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
   AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
