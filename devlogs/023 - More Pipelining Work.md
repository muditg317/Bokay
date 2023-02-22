# DevLog 023 - More Pipelining Work
<div align="right">
February 20th, 2023

Sign on: 6:30pm\
Sign off: 8:45pm
Very distracted work - with friends
</div>

## Goals
- [ ] Make Pipeline class more generic -- allow for more flexible usage
  - [ ] Add template params for first and last stage to be run
  - [x] Fix the compiler error code mapping for calls within `Pipeline`.

## Struggles
- Playing around with templates became extremely confusing and hard to debug
  - Implementing the `<StartStage, EndStage>` logic was very difficult

## Notes
### No major stuff

## Log
- Add `ALL_STAGES` macro usage in `compiler.cpp` to catch `ErrorType` for any stage
  - This should enable future implementation of more complex (but still generic) error handling symantics
- Add compile-time merging of string literals with `StringLiteral` compile-time objects
  - Allows for marginally "more-efficient" stream outputting since string literals get merged

## TODO
- Properly implement/debug the `<StartStage, EndStage>` template params to `Pipeline` class
- Move on to ASTs!!

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- None today!
