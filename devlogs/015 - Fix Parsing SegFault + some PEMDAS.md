# DevLog 015 - Fix Parsing SegFault + PEMDAS
<div align="right">
May 15th, 2022

Sign on: 12:30pm\
Sign off: 1pm\
Also 10pm to 12:30am
</div>

## Goals
- [x] Fix the segfault for the basic-types test
- [ ] Clean code and add comments for interpretability

## Struggles
- Nothing serious today!

## Notes
### No major stuff

## Log
- Mostly just debugging/tracing via gdb
- I fixed it!! - 11:30pm
  - The issue was with scanning for terminal tokens past the EOF (end of file)
  - Random garbage stack value being used causing inconsistent behavior between debug/normal builds b/c out-of-bounds on tokens
  - Issue in ~line 435 (at time-of-writing) of `parser.cpp` - `Token nextToken = tokens[tokInd];`
- Play around with logger prefix during testing to avoid silly mismatches with line numbers of compiler files
- Update correct outputs for all tests
- Start some initial work on PEMDAS for expressions

## TODO
1. Work on parser to correctly parse the full-design test
   1. Or also, work on putting PEMDAS into the grammar

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- Implementing PEMDAS may require reworking `variable_use` and `term`
  - Referencing [Java operator precedence](https://docs.oracle.com/javase/tutorial/java/nutsandbolts/operators.html) and [Java expression precedence](https://www.javatpoint.com/highest-precedence-in-java)