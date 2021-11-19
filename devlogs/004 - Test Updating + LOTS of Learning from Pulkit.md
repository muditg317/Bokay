# DevLog 004 - Test Updating + LOTS of Learning from Pulkit
<div align="right">
November 18th, 2021

Sign on: 2pm\
Sign off: 5pm\
Also 2 to 3am
</div>

## Goals
- Create a script for updating tests
  - Useful to automatically put full paths in the update command

## Struggles
- Making the tests abstract will be hard.
  - Avoiding the full-paths is kinda just unavoidable and hard to solve.
  - It's not something to stress about since I'm likely to be the only dev on this project, but it's a good thing to think about.

## Notes
### Conversations with Pulkit
- Grammar and lexing and parsing and stuff is much much better than just trying to write a by hand find and analyze tokens approach.
  - The former works well for stack-based languages because there are far fewer tokens, each of which is much less complex.
- Not using flex/bison is a huge undertaking because they are so powerful at completing the task of source->tokens->AST.
  - Implementing my own is very much a "this is going to be way way harder but hopefully more rewarding."
#### The steps of compilation:
- Input source code
- Lex into tokens
  - This would find errors related to curly brace mismatches and stuff and invalid tokens / char-sequences.
- Parse tokens into a parse tree or AST
- Parse tree to AST if needed
  - The AST is where we do a lot of the type-checking business.
  - The tree is built top down. Each leaf-node must have a defined type (either a variable with a known type or a literal).
    - If the variable's type is unknown, that's another error. -- A smart compiler might notice an assignment expression missing a type name or infer it's use and then, suggest a type to use.
  - Once the tree is built, type-checking bubble from the bottom up.
    - When a method is called with mismatching args, then the compiler will flag that operation as invalid.
      - (Maybe it assumes which signature was desired for further bubbling?)
    - This is how the compiler can find a bunch of type errors at once rather than crashing/failing after a single type error is seen. The compiler simply bubbles all the way up and finds all the issues.
- Compile the AST
  - I think this is where the code becomes non-platform-independent. Each platform would have to compile to different assembly (unless I don't know what I'm talking about!).
#### The steps of development:
- Choose language features and write some code in the way you want
- Develop a high level grammar that will recognize the different things you want to recognize
  - Not implementation, just rough ideas
  - This indicates what tokens your lexer will need to parse
- Write a lexer that can recognize and classify each of those different tokens
  - Things like struct and enum definitions get a bit odd because your lexer sees an enum definition and now kinda has a new type name it can detect??
- Test the lexer on various outputs and ensure that each of the lexed tokens matches the seen syntax.
  - Dump the lex output to a file (perhaps have a compiler flag). This file would be great for adding to the testing environment.
- Write a parser that takes the tokens and generates a parse tree based on some grammar.
  - This is where we get the nonsense about LL and LR grammars and the difficulty of hand-written grammars.
  - LL parsers can't capture PEMDAS well (pretty much just exponents cause trouble). That's why we might do parse tree first.
    - Doing hybrid parser is possible to avoid doing an entire second step.
- Test this parser on various input files by dumping the parse tree or AST to file as text.
  - Ask Pulkit about that TreeUtils tool that can print a tree with left-aligned level-orders.
  - Also add a compiler flag for dumping this to file (great for testing).
- We didn't dicuss much past this, but I imagine here we get into the sadness of writing assembly for that AST.
#### Lexing
- This will strip out all spaces an new lines (except in strings)
- Extracts tokens and classifies them based on which regex it matched with based on the various token types
 - Each token type has its own regex (I think -- needs more research).
- For comments, I need to do something smart.
 - Perhaps, I can have some preprocessing step that will remove all comments and stuff.
 - `\/\*(\*(?!\/)|[^*]|[\n])*\*\/` is a good starting point for block comment detection.
#### Parse trees / ASTs
- Parse trees are often used because they don't handle PEMDAS or some other criteria.
  - Building the AST from the parse tree is where we would then reintroduce those ordering constraints.
- AST = Abstract Syntax Tree
  - It should only hold syntactic elements.
#### Grammars
- This is simple a set of rules which may or may not be self-referential and recursive.
  - ex: expression = (literal|variable)|(literal|variable)(operator)(expression)
    - Probably a bad/wrong example, but it's a starting point.
- LL grammars are read from left and parsing recursion is finalized also L->R.
  - When we see `public class Hello`, the grammar knows we are now inside of a class (the recursive analysis of the `class` rule is complete).
- LR grammars are read from left but recursive detection of rules is finalized R->L.
  - When we see `public class Hello`, we simply assume that match worked and continue matching. The outer `class` rule match isn't confirmed until we reach the end of the class definition (maybe the last `}`).
  - These are better at doing PEMDAS off the shelf because exponents are "right-something".
  - An LR grammar/parser can be used on an LL language/grammar, but not the other way (needs more research for confirmation).

## Log
- Test updating was not working because the enginePath and testDir didn't match
  - One was full path, but I was using relative path during updates
- Reorganize some #includes for testing
- Created `tests/tester.sh` to run the tester with forwarded params
- Not much development, just discussing with Pulkit (see notes above)

## TODO
1. Get the TreeUtils tool for printing the parse trees and stuff to text file.
2. Brainstorm language features to start thinking about what kind of grammar and tokens I will need.
3. Add compiler flags that can indicate dumping the lexing and parsing outputs to a file.
4. Incorporate the extra compilation steps into the tests (yay, TDD!).

## Language Status/Changes
- Probably won't be stack-based
- Will have multi-step compilation (probably obvious, but hey, I'm learning)

## Unorganized thoughts as they came
- Perhaps add a update compilation only or update execution only.
  - This would allow new devs to only update compilation to reflect full path from their machine.
- Making the tests better is a bit hard unless I ensure the compiler doesn't output full paths.
  - I should stop worrying about this since I'm 99.9999% likely to be the only developer.
- Good thing I'm meeting Pulkit today, I'm going to pick his brain for knowledge and insight into what my development should look like.
- Wow, I learned a lot today.