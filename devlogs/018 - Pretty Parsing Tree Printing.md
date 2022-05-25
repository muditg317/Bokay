# DevLog 018 - Pretty Parsing Tree Printing
<div align="right">
May 24th, 2022

Sign on: 9pm\
Sign off: 10:30pm
</div>

## Goals
- [x] Rewrite the printing of parse trees to be more readable in degenerate cases

## Struggles
- Forgetting to rename variables was annoying because of code duplication
  - Consider refactoring the toTabbedString to be fully recursive
- The confusion regarding `operator=` made it annoying to keep stuff within the same iterative implementation

## Notes
### No major stuff

## Log
- Detect degenerate subtrees within the parsing tree (single child chains)
  - Print these in a single line with ` > ` delimiters

## TODO
1. Add parsing for loops
2. Add parsing for structs/unions

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- The compressed printing is MILES easier to read so this will make debugging rules a lot easier in the future.