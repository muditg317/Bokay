# DevLog 010 - Logging and Lexing
<div align="right">
December 27th, 2021

Sign on: 3pm\
Sign off: 8pm
</div>

## Goals
- [x] Bug-test and semi-finalize lexer
- [x] Add logging lib

## Struggles
- Nothing serious today!

## Notes
### Logging
- Discussion on when to use logging levels:
  - https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
### std::cout neatness with `<iomanip>`
- `#include <iomanip>` gives some cool functionality to `std::cout`
  - Very very basic kinda-ish version of `printf`
  - https://www.cplusplus.com/reference/ios/left/
  - https://www.cplusplus.com/reference/iomanip/
- 

## Log
- Add regex to capture entire comments as single tokens
- Provide line and col numbers to token objects
- Incorporated GLOG
  - Unfortunetely, it logs to stderr, not stdout. oh well
- Use custom prefixes for GLOG logs

## TODO
1. Start work on parser.

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Lexing out the comments seems much easier and would make the parser much simpler (simply throw out comments)
- The default prefix is weird for glog, it will make test output time-dependent.
  - Custom prefix is proving to be somewhat difficult.