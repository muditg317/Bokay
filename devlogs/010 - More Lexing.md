# DevLog 010 - More Lexing
<div align="right">
December 25th, 2021

Sign on: 5pm\
Sign off: 10pm
</div>

## Goals
- [x] Lex/tokenize an entire file!

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Outputs tokens to file when temp outputs are requeested by `-t` flag.
  - Still errors on full-design...
- Errors found - not parsing special characters within the comments of the source
  - Add token types, parser will drop them

## TODO
1. Bug-test the lexer
2. Add logging library!

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- First step should be checking my current progress
  - Create the tokens dump file `.tok`
- Replacing `\\n` with ` ` can cause line number mismatches. Maybe use a `FAKE_NEW_LINE` token or something.
- Consider a priority system for the keywords (might have to leave the "enum" style for types because of extra info)
  - `KW_*` tokens are more important than `WORD`s, but other same length tokens might be in the wrong order elsewhere.