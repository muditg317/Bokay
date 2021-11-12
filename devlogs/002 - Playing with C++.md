# DevLog 000 - Template
<div align="right">
November 11th, 2021

Sign on: 3pm\
Sign off: 5am hopefully
</div>

## Goals
- [ ] Start on the compiler - define its usage
  - Not done at all, kicked until later
- [x] Learn/implement file i/o
- [ ] Research language writing
  - Also kicked until later
- [x] Convert to cmake with CMakeLists

## Struggles
- I don't understand CMake, so I'll stick with `make` with Makefile.
  - find_package(Boost)
- Getting the testing to work is hard
  - Trying to avoid the double coding (one set of test cases that can be run on either engine)

## Notes
### Chat with Pulkit
- Flex and Bison are great for the task
  - flex - lexer
  - bison - build abstract syntax tree
  - Won't do these since they remove some of the exciting parts of development.
- `clang` is a great alternative to `gcc`
- Using `cmake` might be wise compared to `make`/Makefile
### Desired Testing Workflow
- Compile C++ engine
- Test C++ engine on `ROOT/tests`
- Compile Bokay engine with C++ compiler
- Test Bokay engine on `ROOT/tests`
### CMakeLists.txt Learnings
- Install vcpkg to install packages automatically
  - add the toolchain file to the CMakeList configuration args
- add_test can take in generator params for a target executable

## Log
- `Makefile` has been replaced by a series of `CMakeLists.txt`
  - Notes and learnings are detailed above.
- Rudimentary testing workflow has been made that can be run on any engine
  - `test-engine.cpp` is universal
  - `tests/CMakeLists.txt` provides engine targets to the tester

## TODO
1. Add test case files to the tester (yay TDD!)

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Hm, maybe I should use cmake not make
  - jk too hard. I'm so confused
- chat with Pulkit:
  - Flex and Bison are great for lexing and parsing to AST
    - less fun tho since not doin it myself
  - clang is cool
  - cmake is good cuz "normies can't make good Makefile"
- I'm learning CMake and stuff
- Testing process
  - Compile C++ engine
  - Test C++ engine on `ROOT/tests`
  - Compile Bokay engine with C++ compiler
  - Test Bokay engine on `ROOT/tests`
- Trouble with find_package(Boost)
  - It's working, I give up on fixing the cmake warning
- Ay I think most of the `CMakeLists.txt` stuff is working!
- Trying to get testing working and I'm sad
  - Might be working with the `engine-tests.hpp`, but working out kinks/reliability with output checking
- The tests work and can accept an engine target as param! lez go!