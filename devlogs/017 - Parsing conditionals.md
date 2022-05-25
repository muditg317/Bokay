# DevLog 017 - Parsing conditionals + loops
<div align="right">
May 17th, 2022

Sign on: 5pm\
Sign off: 6:30pm\
Also 11:45pm to 12am
</div>

## Goals
- [x] Add grammar rules for conditionals
- [ ] Add grammar rules for loops
- [ ] If finished, start researching AST building

## Struggles
- Writing rules that use left-recursion is challenging sometimes

## Notes
### No major stuff

## Log
- Create rules for conditionals with trying to use left-recursive rules wherever possible
- Revise the test for conditionals to include more complex cases (else-if, nesting, etc)
- Add test case for structs and unions
- Add dynamic test sections for each of the outputs being tested per case
  - Removed this change because it causes the actual compilation to be run once per output type which is unnecessary and doesn't make sense
- Update lexer for base types to include bool (there was no reason to omit it)
- A lot of the conditionals in tests had a `u18` instead of `u8` causing cryptic parsing bugs.

## TODO
1. Rewrite the printing of parse trees to be more readable in degenerate cases
2. Work on parsing for loops (should be simple extension from conditionals)
3. Research AST building

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- I found [some Catch testing docs](https://github.com/catchorg/Catch2/blob/devel/docs/other-macros.md) which have some interesting things which might help with writing more well structured tests.
  - The dynamic section is cool, but it reruns the rest of the test over and over which is not desired for my case.
- The ptree is currently very hard to read because of insane nesting/degenerate cases
  - Consider rewriting the print function to print degenerate portions in one line to avoid the high nesting
  - This would make almost every ptree significantly easier to read sicne stuff would only nest where necessary