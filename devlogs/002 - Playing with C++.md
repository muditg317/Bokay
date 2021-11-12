# DevLog 000 - Template
<div align="right">
November 11th, 2021

Sign on: 3pm\
Sign off: --
</div>

## Goals
- Start on the compiler - define its usage
  - Learn/implement file i/o
- Research language writing

## Struggles
- I don't understand CMake, so I'll stick with `make` with Makefile.

## Notes
### None
- none yet

## TODO
1. 

## Language Status/Changes
- 

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