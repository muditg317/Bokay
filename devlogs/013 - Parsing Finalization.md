# DevLog 013 - Parsing Finalization
<div align="right">
December 29th, 2021

Sign on: 12pm\
Sign off: XXpm/am
</div>

## Goals
- [x] Create better ptree printing
- [ ] Bug-test parsing on `full-design`
- [ ] Generate returnable `ParseTree` if necessary
- [ ] Write ptrees to temp file

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Add `toString`/`toShortString` for `Token`s
  - Shorter prints are nice for the ptree printing.
- Print tree as tabbed string (pretty tree format isn't strictly necessary)
- Write tabbed string of ptree to file and check tests
- Add 0 tokens check to pass `one-comment` test

## TODO
1. Numbered list

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- 