# DevLog 008 - More Designing
<div align="right">
December 23th, 2021

Sign on: 2pm\
Sign off: 7pm
</div>

## Goals
- [x] Continue design process
- [ ] Make decisions about linking process

## Struggles
- Nothing serious today!

## Notes
### Chat with Pulkit about Linking/main function
- Static vs Dynamic Linking
  - Static linking - copy the code each time and recompile it
    - Slower compile times
    - Much easier to implement (No need for a linker)
  - Dynamic linking - have precompiled objects for every importable thing
    - Faster compile times
    - Harder because have to make it work with linking system (and probably header files)
- Dynamic linking is very hard
  - Especially without header files (which I don't want)
### Chat with Pulkit about next steps
- The full-design file outlines a grammar for the language - how things should look
- Now, create a token stream with regex for every different token that is in the language
  - every keyword, equal sign, name (var/func), etc
- Once the tokens are made, then the grammar is made to validate the token stream.
- I had the wrong idea about the lexer
  - It shouldn't be parsing out strings and comments
  - It should essentially split by space and give all tokens with enum of type
  - Lexer knows nothing and should be context-free.

## Log
- Add some more features to full-design test.
- Start work on what tokens will be extracted by the lexer.

## TODO
1. Fix/finish lexer

## Language Status/Changes
- No header files!
- Will probably use static linking

## Unorganized thoughts as they came
- Header files are a hassle to write and maintain.
- I think static linking would be better since working with a linker seems hard.
- I have no strings for now, too hard!
### Parsing/Lexing
- Remove all comments
- Remove all white space (This is why I have no strings lol)
- Apply each regex
- See which one yields longest match
- Extract that from string, repeat