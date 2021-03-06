#ifndef LEXER_HPP
#define LEXER_HPP

#include "args-parser.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <regex>


enum class LexerResult {
  LEXING_SUCCESS,
  INVALID_LEXING_OPTIONS,
  INVALID_TOKENS,
};

enum class TokenType : char {
  WORD,
  SEMICOLON, // ; -- used for line ending
  KW_IMPORT,
  COMMA, // , -- used for param separation and delimiting imports
  KW_FROM,
  IMPORT_SOURCE, // relative path as a string
  COLON, // : -- used for :: in `FullImportedLibrary::aMethodInLib`
  BASE_TYPE, // integer or floating point types
  DECIMAL_LITERAL,
  // HEX_LITERAL,
  // OCTAL_LITERAL,
  // BINARY_LITERAL,
  FLOATING_POINT_LITERAL,
  OPEN_BRACKET,CLOSE_BRACKET, // []
  // ARRAY_DECL, // the [5] in `u8 arr[5]` -- maybe just use open[ + num_lit + close]
  EQUALS,
  PLUS,   MINUS,   MULT,   DIV,   MOD,
  PLUS_EQ,MINUS_EQ,MULT_EQ,DIV_EQ,MOD_EQ,
  KW_STRUCT,KW_UNION,
  PERIOD, // . used for member access -- . in `struct_var.member`
  OPEN_BRACE,CLOSE_BRACE, // {}
  LINE_COMMENT, // // used for line comment starting
  OPEN_BLK_CMT, // /*
  CLOSE_BLK_CMT, // */
  OPEN_PAREN,CLOSE_PAREN, // ()
  KW_RETURN,
  KW_IF,
  KW_WHILE,
  DOUBLE_EQ, // ==
  TRIPLE_EQ, // ===
  LSS,GTR,
  LEQ,GEQ,
  WHITESPACE,
  NUM_TOKEN_TYPES, // counter for enum types
};

std::ostream& operator<< (std::ostream& out, const TokenType& type);

class Token {
  public:
    Token(std::string str, TokenType tok_type): contents(str), type(tok_type) {};
    std::string getContents(void) {
      return contents;
    }
    TokenType getType(void) {
      return type;
    }
  private:
    std::string contents;
    TokenType type;
};

class Lexer {
  public:
    Lexer(Options &options);
    LexerResult run(std::string sourceCode, std::vector<Token> &resultTokens);
  private:
    bool validateOptionsAndSource(std::string sourceCode);
    std::string preprocessSource(std::string sourceCode);
};

#endif