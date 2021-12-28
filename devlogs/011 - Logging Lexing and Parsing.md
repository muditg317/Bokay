# DevLog 010 - Logging Lexing and Parsing
<div align="right">
December 27th, 2021

Sign on: 3pm\
Sign off: 8pm\
Also 12 to 1:30am.
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
- Making the parser rules semi-dynamic to have either `Token` values or other `ParseNodeType` values
  - Tomorrow problem!

## Notes
### Logging
- Discussion on when to use logging levels:
  - https://stackoverflow.com/questions/2031163/when-to-use-the-different-log-levels
### std::cout neatness with `<iomanip>`
- `#include <iomanip>` gives some cool functionality to `std::cout`
  - Very very basic kinda-ish version of `printf`
  - https://www.cplusplus.com/reference/ios/left/
  - https://www.cplusplus.com/reference/iomanip/
### Fixed but dynamic size container
- I need fixed size containers, but it's not known until runtime
  - Technically it is compile time, but it's a jagged array.
- https://stackoverflow.com/questions/14895052/container-of-fixed-dynamic-size
- `const std::vector<T>` works because the `const` makes it unable to change size and stuff
### Arrow notation in function declaration
- Functionally the same as normal syntax but mre powerful
- Used to provide return type based on arguments
- https://stackoverflow.com/questions/22514855/arrow-operator-in-function-heading/22515589
```C++
template <typename T1, typename T2>
auto compose(T1 a, T2 b) -> decltype(a + b);
```

## Log
- Add regex to capture entire comments as single tokens
- Provide line and col numbers to token objects
- Incorporated GLOG
  - Unfortunetely, it logs to stderr, not stdout. oh well
- Use custom prefixes for GLOG logs
- Created baseline infrastructure (very subject to change) for the parser implementation
  - This setup will have to drastically change to account for the above eplained "struggle".

## TODO
1. Start work on parser.
   1. Started today!
2. Make my devlogs more detailed in the future.
3. Restructure parser things.

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Lexing out the comments seems much easier and would make the parser much simpler (simply throw out comments)
- The default prefix is weird for glog, it will make test output time-dependent.
  - Custom prefix is proving to be somewhat difficult.
- Implementing the parser as a regex could be interesting.