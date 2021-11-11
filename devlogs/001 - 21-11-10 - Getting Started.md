# DevLog 001 - Getting Started
<div align="right">
November 10th, 2021

Sign on: 8pm maybe\
Sign off: 5am oof
</div>

## Goals
- Decide on some language info (detailed below)
- Setup the repo

## Log
- Chose Bokay as name
- `engines/` will hold the various compilers for the language
  - initially cpp, eventually itself
- `tests/` will hold a bunch of unit tests for the language (test compilation and language features, not function implementations)
- `compile.cpp` should build to a single executable with proposed usage:
  - ./compile [engine] [command] [file] [-c ...args] [-r ...args]
    - engine: cpp | bokay
    - command: compile | run (will compile first)
    - file: path to `.bokay` file
    - -c args: args to pass to compiler
    - -r args: args to pass to compiled code
- I finally got `engines/cpp/Makefile` working <3am>, yay!
- I've decided to start making a devlog for this project!


## Struggles
- Learn how C++ works
  - I kinda understand the C stuff, but the OOP is new territory.
- Better understanding of C/C++ compilation
  - https://www.youtube.com/watch?v=ksJ9bdSX5Yo
- Learn `make` in order to write good Makefile
  - https://stackoverflow.com/questions/53136024/makefile-to-compile-all-c-files-without-needing-to-specify-them/53138757
- I know nothing about language or compiler writing lol

## Notes
### [C/C++ Compilation Process](https://www.youtube.com/watch?v=ksJ9bdSX5Yo)
- [TODO: rewatch and add notes]

## TODO
1. Research language architecture/creation

## Language Status
- Name: Bokay
  - Suggestion from Adithya :)
- Base compiler: C++
  - Compiled with make
- Self-hosted
- Typing: static
  - I'd never make a dynamically typed language
- Stack-based vs something else??: idk
- Turing-complete: probably/yes/I need to research
