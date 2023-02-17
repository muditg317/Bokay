# DevLog 021 - Import-Export Syntax + Abstractions + ASTs
<div align="right">
February 17th, 2023

Sign on: 4:45pm\
Sign off: 7:00pm
Also 9:45 - 10:15pm
</div>

## Goals
- [x] Add tests for import/export syntax
- [ ] Begin ParseTree -> AST reduction implementation

## Struggles
- Got carrier away trying to get template string literals for `CompilerStage` names
  - Using lots of stack overflow but nothing working yet
  - Made some progress with static functions
  - Finally made it work with C++20 based compile-time string literals

## Notes
- Found some good reference for compile-time strings on [this stack overflow post](https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/15863804#15863804)
- C++20 standard is so good, wow -- allows for compile-time string literals in templates as described in [this blog](https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/)

## Log
- Create test for import/export syntax
  - Only testing import currently, export not yet implemented
- Add abstract base class for compiler stages to adhere to (not sure this is beneficial but maybe it'll help with code consistency)
  - Convert lexer,parser,ast-builder to inherit from this base class
- Work on templated c_str for `CompilerStage` names
  - Used compile-time string template support based on C++20 standard

## TODO
- Actually do AST building (keep getting side-tracked)

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- None today!