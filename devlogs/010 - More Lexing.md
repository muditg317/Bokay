# DevLog 010 - More Lexing
<div align="right">
December 25th, 2021

Sign on: 5pm\
Sign off: XXpm/am
</div>

## Goals
- [ ] Lex/tokenize an entire file!

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Outputs tokens to file when temp outputs are requeested by `-t` flag.
  - Still errors on full-design...

## TODO
1. Numbered list

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- First step should be checking my current progress
  - Create the tokens dump file `.tok`
- Replacing `\\n` with ` ` can cause line number mismatches. Maybe use a `FAKE_NEW_LINE` token or something.