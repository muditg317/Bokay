# DevLog 005 - Add structure for lexing-parsing-etc
<div align="right">
November 29th-30th, 2021\

Sign on: 4pm\
<!-- Sign off: XXpm/am -->
</div>

## Goals
- [x] Add testing structure for substeps of compilation
  - [x] Lexing
  - [x] Parsing
    - [x] AST conversion
  - [x] Intermediate repr/blocks
  - [ ] Machine specific compilation?
- [x] Setup command line parsing in the c++ engine

## Struggles
- Nothing serious today!

## Notes
### Conversation with Pulkit from last night
- After AST, you create blocks of code represented with 3-op assembly
  - Each block is a contiguous stream of instructions with no branches (except entry/exit from block)
- Blocks would be stored as a graph
- From here, figure out the control flow of the whole program

## Log
- Create infrastructure for testing each of the subcomponents of the compilation process
- Branch off for feature development (add compiler flags and stuff)
  - Create sub libraries within the CPP engine for various steps
- Add compiler flags and arguments to C++ engine for Bokay

## TODO
1. Start work on the compiler itself
2. Research more things
   1. 3-op assembly
   2. How to create a "main" entry point for the executable

## Language Status/Changes
- Will be C-style
  - Objects are hard
  - Imperative/functional seems to be way easier from the blocks/control-flow aspect

## Unorganized thoughts as they came
- This project seems harder and harder as I continue working.
- 