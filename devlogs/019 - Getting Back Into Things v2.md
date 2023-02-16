# DevLog 019 - Getting Back Into Things v2
<div align="right">
February 15th, 2023

Also 2am-3am - slow progress
Sign on: 6:30pm\
Sign off: 7:30pm
Also 2/16: 1:30am-4:30am - semi-distracted/just slow since getting into it
</div>

## Goals
- [x] Reimport project to new laptop
  - [x] Update `vcpkg` deps and ensure compilation works
- [ ] Re-understand codebase
  - Probably not going to happen -> Work on AST assuming previous steps just work
- [ ] Begin work on AST generation (essentially reduced/compressed representation of parsing-tree)

## Struggles
- `cmake` with `vcpkg` deps was a bit annoying toolchain dependencies/etc
  - using VSCode with CMake Tools extension helped and hurt
- Some Boost changes/deprecations made things annoying
- Generic `ASTNode` class caused a lot of issues with vectors/iterators/pairs etc
  - Especially complicated implementation of `ASTNode::toTabbedString()`
- Testing is broken -- getting weird `undefined reference to google::InitGoogleLogging(char const*, void (*)(std::ostream&, google::LogMessageInfo const&, void*), void*)` errors
  - Tried some stuff with `CMakeLists.txt` options but no luck

## Notes
- CMake 3.24.2 solves the silly "Boost new versions dependencies may be included..." warning
  - apt repository outdated -- still at 3.22.1
- Got some AST inspiration from [stack-overflow AST post](https://stackoverflow.com/questions/28530284/generate-an-ast-in-c)
- `std::vector` must have non-const non-reference type
  - Learned this many times over
  - Can somewhat get around this with `std::vector<std::reference_wrapper<T>>`
- Looked a lot into build process of vcpkg libraries to try understanding why glog compiled with no custom prefix support -- too complex and abstracted :(

## Log
- Reconfigure VSCode cmake settings
- Fix compilation errors
  - deprecated `boost::filesystem::fstream` -> `std::fstream`
  - `TokenType` comparison with `map.size` requires `size_t` static cast
  - `catch2` library main include is now `catch2/catch_all.hpp` (was `catch2/catch.hpp`)
- Start building out infrastructure for AST building (`ast-builder.hpp` & `ast-builder.cpp`)
  - Initial layout largely based on `parser.hpp`/`parser.cpp`
- `ASTNode` abstract class provides structure

## TODO
1. Look into `export` block for `bokay` source code
2. Start implementing AST generation
  1. NEED to refactor away the `children` member-field of `ASTNode` (causes huge headaches with abstract class etc -- `dynamic_cast(subclass)` may be even worse unfortunately)
3. Testing is broken because custom prefixes for GLOG is broken
  1. Need to fix this ASAP

## Language Status/Changes
- Maybe prefer slower compilation in exchange for smaller binaries
  - very forward thought -- recompile any imported symbols iff used in code body

## Unorganized thoughts as they came
- None so far!