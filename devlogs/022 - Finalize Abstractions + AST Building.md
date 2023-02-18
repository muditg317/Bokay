# DevLog 022 - Finalize Abstractions + AST Building
<div align="right">
February 18th, 2023

Briefly at 4:45pm - 5:00pm
Sign on: 8:30pm\
Sign off: 5:15pm
</div>

## Goals
- [ ] Finish up work related to abstracting compiler stages
- [ ] Begin ParseTree -> AST reduction implementation

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Move debug callback logic to `CompilerStage` class
  - Having the `#ifdef`s sprinkled around looked messy
- Create generic mapping structure for compile-time mapping of `CompilerStage` to other fields
  - Used `CompilerStage`->`CompilerResult` error-code mapping to replace template param for `runStage` function


## TODO
- 

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Using more absractions in the compiler could allow for simply chaining the stages together (one quick loop).
  - This would make it much more seamless to add new stages without having to add lots of code.
  - Can use a compile-time type map between stages and other fields (compiler error code, file ext, etc).
  - Got some ideas on compile-time mappings based on [this StackOverflow answer](https://stackoverflow.com/a/58368483)