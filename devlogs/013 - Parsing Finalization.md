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
- [x] Write ptrees to temp file

## Struggles
- Bug: Nested rules don't work past 1 level of nesting
  - The parser doesn't add duplicated `ParsingState` (potential matches)
  - Ex: Already have `expr op term` from t=3 with 1 level, won't add another `expr op term` from t=3 even though it's more nested.
  - Fix options:
    - equality check will iterate over the tree
    - force add children from completions

## Notes
### No major stuff

## Log
- Add `toString`/`toShortString` for `Token`s
  - Shorter prints are nice for the ptree printing.
- Print tree as tabbed string (pretty tree format isn't strictly necessary)
- Write tabbed string of ptree to file and check tests
- Add 0 tokens check to pass `one-comment` test
- Add test for complicated arithmetic expressions
- Fix rule nesting bugs
- 

## TODO
1. Numbered list

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- PEMDAS is best implemented within the parser not the ptree->AST