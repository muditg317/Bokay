#ifndef LEXER_HPP
#define LEXER_HPP

#include "args-parser.hpp"

#include <string>
#include <vector>


enum LexerResult : size_t {
  LEXING_SUCCESS,
  INVALID_LEXING_OPTIONS,
  INVALID_TOKENS,
};

class Token {
  public:
    Token(std::string str): contents(str) {};
    std::string getContents(void) {
      return contents;
    }
  private:
    std::string contents;
};

class Lexer {
  public:
    Lexer(Options &options);
    LexerResult run(std::string sourceCode, std::vector<Token> &resultTokens);
  private:
    bool validateOptionsAndSource(std::string sourceCode);
};

#endif