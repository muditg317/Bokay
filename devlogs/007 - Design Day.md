# DevLog 007 - Design Day!
<div align="right">
December 14th, 2021

Sign on: 7pm\
Sign off: ??
</div>

## Goals
- [ ] Design the language!

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Convert `tests/compiles.bokay` to `tests/full-design.bokay` with various design choices outlined.
- Add `tests/basic-types.bokay`.

## TODO
1. Continue designing features

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- I want import structure similar to JS/TS with babel "compilation"
  - `import WholeLib, {oneMethod, SomeClass, anotherMethod} from "file/to/import.bokay"`;
- The union types for u64 is very cool in HolyC from TempleOS by Terry
  - `i64` is unioned with `u8[8]`, `u16[4]`, etc.
  - This would be nice to hae but may be hard.