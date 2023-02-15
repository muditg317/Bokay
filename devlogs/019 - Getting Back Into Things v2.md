# DevLog 019 - Getting Back Into Things v2
<div align="right">
February 15th, 2023

Also 2am-3am - slow progress
Sign on: 6:30pm\
Sign off: pm
</div>

## Goals
- [x] Reimport project to new laptop
  - [x] Update `vcpkg` deps and ensure compilation works
- [ ] Re-understand codebase
- [ ] Begin work on AST generation (essentially reduced/compressed representation of parsing-tree)

## Struggles
- `cmake` with `vcpkg` deps was a bit annoying toolchain dependencies/etc
  - using VSCode with CMake Tools extension helped and hurt
- Some Boost changes/deprecations made things annoying

## Notes
- CMake 3.24.2 solves the silly "Boost new versions dependencies may be included..." warning
  - apt repository outdated -- still at 3.22.1

## Log
- Reconfigure VSCode cmake settings
- Fix compilation errors
  - deprecated `boost::filesystem::fstream` -> `std::fstream`
  - `TokenType` comparison with `map.size` requires `size_t` static cast
  - `catch2` library main include is now `catch2/catch_all.hpp` (was `catch2/catch.hpp`)

## TODO
1. None so far!

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- None so far!