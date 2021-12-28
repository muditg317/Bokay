# DevLog 010 - Logging Lexing and Parsing
<div align="right">
December 27th, 2021

Sign on: 3pm\
Sign off: 8pm\
Also 12 to __am.
</div>

## Goals
- [x] Bug-test and semi-finalize lexer
- [x] Add logging lib
- Bonus goals
  - [x] Start parsing!

## Struggles
- Implementing custom prefixes was a bit annoying.
  - `vcpkg` has some features/options functionality that allowed me to activate the `"WITH_CUSTOM_PREFIXES"` options.
  - This then enables the `CustomPrefix` function handler.

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
- Created baseline infrastructure (very subject to change) for the parser implementation

## TODO
1. Start work on parser.
   1. Started today!
2. Make my devlogs more detailed in the future.

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Lexing out the comments seems much easier and would make the parser much simpler (simply throw out comments)
- The default prefix is weird for glog, it will make test output time-dependent.
  - Custom prefix is proving to be somewhat difficult.
- Implementing the parser as a regex could be interesting.