# DevLog 003 - Adding Tests
<div align="right">
November 12th, 2021

Sign on: 2:30pm\
Sign off: 11:30pm
</div>

## Goals
- Finish testing setup
  - ways to run tests, update test output, etc

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Test running with file i/o works with `ProgramOutput` to store the compilation and execution output.
- Tests can be updated with `--update` on `test-engine`.

## TODO
1. Start researching language design and start working on `engines/cpp`.

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- `tests` folder will have a series of `.bokay` files in it
  - subfolder `__SNAPSHOTS__` will contain correct output for compilation and running
- It works with file i/o!
  - I need to make a test updater, so I can automatically define correct outputs.
  - Capturing stderr may be a problem.
- Running the program through the engine doesn't make sense, just call the compiled executable.
  - Also, doing that would force me to make the language capable of calling shell commands.
- Tests can be updated via command line!
  - `--update [testCase1;testCase2|all]
- Correct test output as `testName.bokay.test`