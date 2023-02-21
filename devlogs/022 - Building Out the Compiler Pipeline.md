# DevLog 022 - Building Out the Compiler Pipeline
<div align="right">
February 18th, 2023

Briefly at 4:45pm - 5:00pm
Sign on: 8:30pm\
Sign off: 7:30am
Pretty slow working in the first 6 hours -- lots of dead time
</div>

## Goals
- [ ] Finish up work related to abstracting compiler stages
- [ ] Begin ParseTree -> AST reduction implementation

## Struggles
- Playing with tuples and compile-time parameter packs and fold expressions -- very confusing and sad
- Moving all the stage logic to generic `Pipeline` class proved quite difficult
  - Building the `next_stage_for` map at compile time was quite a pain

## Notes
### Compile-time Macros / Template Quirks
- Run compiler with just preprocessor to debug macros working
  - `clang++ -E <src>.hpp | test.hpp`
- can't put `#define` inside another `#define`
- compile-time string comparison:
```cpp
constexpr bool strings_equal(char const * a, char const * b) {
    return *a == *b && (*a == '\0' || strings_equal(a + 1, b + 1));
}
```
### C++ knowledge
- `std::tuple_size_v` -- get number of elements of tuple
- default capture-all by reference for lambdas -- only captures used variables
  - no need to worry about "inefficiency" of capturing "everything"

## Log
- Move debug callback logic to `CompilerStage` class
  - Having the `#ifdef`s sprinkled around looked messy
- Create generic mapping structure for compile-time mapping of `CompilerStage` to other fields
  - Used `CompilerStage`->`CompilerResult` error-code mapping to replace template param for `runStage` function
- Setup tuple of `CompilerStage` objects to be used in `Compiler` class
  - No longer need to pass in the `Stage` reference -- just `std::get` from the tuple
- Configure compile-time mapping to use direct `CompilerStage` classes rather than `StringLiteral` references to `Stage::NAME`
  - This allows for cleaner usage of map lookup with just the `CompilerStage` typename
- Implement automatic `runStages` function that can run the entire pipeline
- Create macros for generating the compile-time mappings -- easier extensibility with more stages
- Cleanup include/header files + add `#pragma once` to all headers
  - Moved `CompilerStage` to separate header
  - Migrate compile-time mapping templates/macros into `utilities.hpp` header
- Extract `Pipeline` class to wrap logic of connected stages
  - Create compile-time generated mapping for `next_stage_for` based on `Stages...` template param
  - Add `PipelineDataTuple` to hold all inputs + final output --> more seamless `runStages` function

## TODO
- Make the pipeline take template params for the first and last stage to be run
  - This would allow more seamless subcalls to the pipeline (for compiling imported modules during base compilation)
- Fix the compiler error code mapping for calls within `Pipeline`.
- Still need to do ASTs

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Using more absractions in the compiler could allow for simply chaining the stages together (one quick loop).
  - This would make it much more seamless to add new stages without having to add lots of code.
  - Can use a compile-time type map between stages and other fields (compiler error code, file ext, etc).
  - Got some ideas on compile-time mappings based on [this StackOverflow answer](https://stackoverflow.com/a/58368483)
- Lots of dead-time during development today
  - just reading cppreference.com
  - repeated failed compilations while attempting to figure out template quirks