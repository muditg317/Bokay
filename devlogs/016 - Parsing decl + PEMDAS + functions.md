# DevLog 016 - Parsing decl + PEMDAS + functions
<div align="right">
May 16th, 2022

Sign on: 2:30pm\
Sign off: 7pm\
Also 11:30pm to 3am
</div>

## Goals
- [x] Modify grammar to parse decls and correctly handle PEMDAS
- [x] Add parsing rules for functions and argument lists

## Struggles
- Nothing serious today!

## Notes
### Earley Parser
- The Earley parsing algorithm can handle left or right recursion
  - It's just better with left, but either will be recognized
### VSCode settings
- Environment variables can be used in vscode `settings.json` file via `${env:VAR_NAME}`
  - However, VSCode has to be launched from a terminal with those environment variables set

## Log
- Add various split up tests for more broken down pieces of the language (reduce dependence on `full-design.bokay`)
- Reorganize grammar rules
  - statements are either decl/expr
  - declarations are split into base type and declarator/initializer list
  - literals reorganized + add boolean literal
  - expressions/operators follow PEMDAS roughly based on C++
- Move grammar rules and related classes to `grammar.cpp` within LibParser
- Update test-utils to delete build files from previous test runs to ensure correct test running/updating
- Create rules for parsing functions, func decl param lists, function params lists, argument lists for function calls
  - Functions test now passes parsing

## TODO
1. Code cleaning/commenting was never done
   1. May be pushed for later since I was able to get back into it without much difficulty
2. Setup configure/build/run scripts for this cmake project
   1. (favored over reliance on the VSCode plugin)
3. Add parsing for loops and conditionals

## Language Status/Changes
- The language may or may not now support pointers and stuff.

## Unorganized thoughts as they came
- The C/C++ decl style seems very robust, so I should just try to emulate that
  - Referencing the [declaration syntax breakdown](https://en.cppreference.com/w/cpp/language/declaration)
  - Also [array declaration reference](https://en.cppreference.com/w/cpp/language/array)
- Java doesn't have some language features - prefer the C++ PEMDAS
  - [C++ order of operations](https://en.cppreference.com/w/cpp/language/operator_precedence)
- I needa stop copy pasting C++. oops that was the whole point of this project
- Consider some sort of "failed but best ptree" printout to help with debugging full-design
  - This is partially accomplished by the broken down tests of individual language features
- The `ARG_LIST` grammar rule for function calls was hard to tune and fails for trailing commas
  - This can simply be a language feature since trailing commas don't make sense in function calls
    - (last params using defualts -> just don't specify them)
- The VCPKG_ROOT environment var can be passed to vscode settings for easier configuration of toolchain file path
  - Could be worked around by moving to build/run scripts instead of relying on vscode plugin
  - This may be better because scripts would hopefully also be more portable