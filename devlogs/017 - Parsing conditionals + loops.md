# DevLog 017 - Parsing conditionals + loops
<div align="right">
May 17th, 2022

Sign on: 5pm\
Sign off: XXpm/am
</div>

## Goals
- [ ] Add grammar rules for conditionals
- [ ] If finished, start researching AST building

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Create rules for conditionals with trying to use left-recursive rules wherever possible
- Revise the test for conditionals to include more complex cases (else-if, nesting, etc)
- Add test case for structs and unions
- Add dynamic test sections for each of the outputs being tested per case
  - Removed this change because it causes the actual compilation to be run once per output type which is unnecessary and doesn't make sense

## TODO
1. Numbered list

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- I found [some Catch testing docs](https://github.com/catchorg/Catch2/blob/devel/docs/other-macros.md) which have some interesting things which might help with writing more well structured tests.
  - The dynamic section is cool, but it reruns the rest of the test over and over which is not desired for my case.