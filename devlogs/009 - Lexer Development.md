# DevLog 009 - Lexer Development
<div align="right">
December 24th, 2021

Sign on: 7pm\
Sign off: XXpm/am
</div>

## Goals
- [ ] Fix the lexer to not parse larger items at a time
- [ ] Finish the lexing implementation to return a stream of tokens

## Struggles
- Nothing serious today!

## Notes
### Research on Lexing and Parsing
- Lexers can remove comments
- Lexers can/should? parse the strings
- Using regex is a common practice for lexers

## Log
- Update some regex values
- Add regexes for start/end block comment
- Make token types more generic (specificity will come from parser)
- Add stderr capture to `execShellCommand`
- Lexing tokens seems to be rouhgly working! Improvements to come later.

## TODO
1. Setup `glog` for more efficient logging
2. Continue development on/refine the lexer.

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Perhaps I'll disallow weirdly placed comments to avoid the issue with strings.
- I think I need to start using a logging library for all the debugging work.
  - `glog` by Google looks quite good: https://github.com/google/glog