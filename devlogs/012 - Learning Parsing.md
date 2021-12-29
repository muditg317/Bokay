# DevLog 012 - Learning Parsing
<div align="right">
December 28th, 2021

Sign on: 1pm\
Sign off: XXpm/am
</div>

## Goals
- [x] Learn about parsers and techniques
- [ ] Start writing a parser

## Struggles
- Nothing serious today!

## Notes
### Parsing
- Earley parsing algorithm
  - Good for left-recursive grammars
  - Not limited like LL/LR grammars?
  - [Wiki](https://en.wikipedia.org/wiki/Earley_parser)
  - Variation that is faster/better?
    - https://link.springer.com/content/pdf/10.1007%2F3-540-61053-7_68.pdf
- Algorithm comparison
  - https://stereobooster.com/posts/an-overview-of-parsing-algorithms/
### Random Notes
- Can't have `const` constructors because const implies that members are not changed
  - Purpose of constructor is to initialize (change) all members
- `std::variant<>` requires std > ++17
- Some good info on `static_cast` and `dynamic_cast`: https://stackoverflow.com/questions/8469900/cant-downcast-because-class-is-not-polymorphic
  - A class needs virtual methods to do dynamic casting
  - Dynamic cast uses RTTI (run-time type info) to perform a runtime check on cast validity.

## Log
- Did a bunch of research about parsing
- Created new structure for parser
  - Uses Earley parsing algo
- Implement tree building as the match states are built up
- Create all the grammar rules (subject to change)

## TODO
1. Numbered list

## Language Status/Changes
- None today!

## Unorganized thoughts as they came
- I know nothing, so the best course of action is probably to jsut research online about parsers.
- I'll probably go with some version of Earley's algorithm
  - I don't want to copy-paste an existing algorithm, so I'm just going to jump in and work.
- I can provide meaningful `invalid token` messages by waiting until lexing completion to error and provide all invalid tokens.