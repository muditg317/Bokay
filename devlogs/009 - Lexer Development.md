# DevLog 009 - Lexer Development
<div align="right">
December 24th, 2021

Sign on: 7pm\
Sign off: 1am
</div>

## Goals
- [x] Fix the lexer to not parse larger items at a time
  - Cancelled based on research. Continue parsing strings, but allow comments to pass to parser.
- [x] Finish the lexing implementation to return a stream of tokens
  - Still needs bug fixing and improvements.

## Struggles
- Nothing serious today!

## Notes
### Research on Lexing and Parsing
- Lexers can remove comments
- Lexers can/should? parse the strings
- Using regex is a common practice for lexers
### Capturing stdout and stderr
- Both outputs can be captured by using a tempfile.
  - Direct the command's stderr to file. Then read that file. stdout is already captured via pipe.
  - `cmd` --> `cmd 2> tmpstderr.txt`

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