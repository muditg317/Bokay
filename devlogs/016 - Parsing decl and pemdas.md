# DevLog 016 - Parsing decl and pemdas
<div align="right">
May 16th, 2022

Sign on: 2:30pm\
Sign off: 7pm
</div>

## Goals
- [ ] Modify grammar to parse decls and correctly handle PEMDAS

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Add various split up tests for more broken down pieces of the language (reduce dependence on `full-design.bokay`)
- Reorganize grammar rules
  - statements are either decl/expr
  - declarations are split into base type and declarator/initializer list
  - literals reorganized + add boolean literal
  - expressions/operators follow PEMDAS roughly based on C++
- Move grammar rules and related classes to `grammar.cpp` within LibParser

## TODO
1. Numbered list

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- The C/C++ decl style seems very robust, so I should just try to emulate that
  - Referencing the [declaration syntax breakdown](https://en.cppreference.com/w/cpp/language/declaration)
  - Also [array declaration reference](https://en.cppreference.com/w/cpp/language/array)
- Java doesn't have some language features - prefer the C++ PEMDAS
  - [C++ order of operations](https://en.cppreference.com/w/cpp/language/operator_precedence)
- I needa stop copy pasting C++. oops that was the whole point of this project