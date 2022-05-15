# DevLog 014 - Getting Back Into Things
<div align="right">
May 14th, 2022

Sign on: 9pm PST!\
Sign off: 1:30am\
Very distracted work - maybe 1-2 hours dev time
</div>

## Goals
- [x] Get compiling/running working again
- [ ] Read through existing code and add comments wherever its hard to remember what does what
- [x] Add setup script for vcpkg
  - may not be completely necessary but will be helpful

## Struggles
- A bunch of tests are now failing for unknown reasons
  - It seems that the tests only segfault when run from the testing engine
  - They still have incorrect output anyways
- The segfault for basic-types seems very weird
  - The `existing == newState` check seems to have `existing` as a broken value maybe?

## Notes
### No major stuff

## Log
- Got confused by Boost version warning in cmake output
  - Need to fix this warning
- Setup for useful environment scripts for the project
  - See the `scripts/` directory
- Updated `cmake` to be installed via `pip` for better version management
- CTests are now failing with some weird segfaults and tokens arent being parsed correctly
  - Turns out that the code has just been updated and tests don't have the updates. Segfault is still an issue.
  - Current test expected outputs don't have anything for parser output

## TODO
1. Fix the segfaulting for the basic-types test
2. Cleanup code if needed and add comments to make sure its interpretable to new people (basically myself right now since long break)


## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- 