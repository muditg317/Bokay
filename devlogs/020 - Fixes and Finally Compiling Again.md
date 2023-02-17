# DevLog 020 - Fixes and Finally Compiling Again
<div align="right">
February 16th, 2023

Sign on: 5:15pm\
Sign off: 7:00pm
Also 5:30am - 6:30am
</div>

## Goals
- [x] Fix compilation errors with glog
- [ ] Understand+Fix remianing parser failures
  - [ ] loops
  - [ ] structs/unions
  - [ ] full design
- [ ] Begin work on AST generation (essentially reduced/compressed representation of parsing-tree)

## Struggles
- Understanding the inner workings of `vcpkg` took a good bit of digging and debugging.
- Updating grammar requires quite a few other changes (initially forgot to actually include reference to `LOOP` productions from the `statement` production)

## Notes
- `vcpkg` log files revealed a lot about the inner process
  1. packages are downloaded into `$VCPKG_ROOT/buildtrees/<package-name>/src/`
  2. packages are configured into `$VCPKG_ROOT/buildtrees/<package-name>/<triplet>-[rel|dbg]/` via command in `config-<triplet>-out.log` -- change this command as necessary
  3. packages are built -- no log file: I used command `cmake --build . --config [Debug|Release] --target install --` to build
  4. packages are installed into `$VCPKG_ROOT/packages/<package-name>_<triplet>/[debug]?` via command in `install-<triplet>-[rel|dbg]-out.log` -- installs the headers + compiled lib
  5. compiled lib (`lib<package>.a`) is saved into installation directory (default= `$VCPKG_ROOT/installed/<triplet>/[debug]?/lib` -- no logs here, simply used `cp` based on code seen in [vcpkg-tool source](https://github.com/microsoft/vcpkg-tool/blob/3e20dcc195bfa7d6c204f95c60c496d33d629eb5/src/vcpkg/install.cpp#L115)

## Log
- Dug through vcpkg log files to figure out how exactly libraries were configured/built/installed
- Recompiled glog with custom commands and replicating vcpkg behaviors
- Compilation of cpp-compiler now works!
- Update expected test outputs -- `compiler.cpp` had some small line-number changes
- Add support for loops
  - Add lexer tokens for `do`/`break`/`continue`
  - Add grammar rules for while/do-while loops
- Allow functions to take any type of declarator as return type / parameter type
  - Required test updates for `functions.bokay`
- Update `loops` tests to include correct output for `ParseTree` generation

## TODO
1. Finish parsing errors before moving to AST generation

## Language Status/Changes
- Consider support for parameter groups -- untyped parameters inherit the type of the preceding parameter (e.g. `int a, b, c;`)
- Functions previously only had `no_ptr_decl` parameters -- now can have any `declarator` parameter
  - not sure why it was `no_ptr_decl` initially

## Unorganized thoughts as they came
- Wow, using `objdump -D` helped A LOT for determining if the second implementation of `google::InitGoogleLogging` was being compiled
  - Thank you to CS6035 (Info Sec) project 1 for teaching me about `objdump`!