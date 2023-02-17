# DevLog 021 - Import-Export Syntactical Testing + ASTs
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

## Notes
- Found some good reference for compile-time strings on [this stack overflow post](https://stackoverflow.com/questions/15858141/conveniently-declaring-compile-time-strings-in-c/15863804#15863804)

## Log
- Create test for import/export syntax
  - Only testing import currently, export not yet implemented
- Add abstract base class for compiler stages to adhere to (not sure this is beneficial but maybe it'll help with code consistency)
  - Convert lexer,parser,ast-builder to inherit from this base class
- Work on templated c_str for `CompilerStage` names
  - Not working yet, but I'm getting closer
  - Made progress by just making static functions for stage name which used templated value (instead of const-static field)

## TODO
- Actually do AST building (keep getting side-tracked)

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- None today!