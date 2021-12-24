#ifndef LEXER_HPP
#define LEXER_HPP

#include "args-parser.hpp"

#include <string>
#include <vector>
#include <regex>


enum class LexerResult {
  LEXING_SUCCESS,
  INVALID_LEXING_OPTIONS,
  INVALID_TOKENS,
};

enum class TokenType : size_t {
  NAME,
  LINE_ENDING, // ;
  KW_IMPORT,
  IMPORT_DELIM, // ,
  KW_FROM,
  IMPORT_SOURCE, // relative path as a string
  LIB_ACCESSOR, // :: in `FullImportedLibrary::aMethodInLib`
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
  MEMBER_ACCESSOR, // . in `struct_var.member`
  OPEN_BRACE,CLOSE_BRACE, // {}
  // COMMENT_STARTER, // //
  OPEN_PAREN,CLOSE_PAREN, // ()
  KW_RETURN,
  KW_IF,
  KW_WHILE,
  DOUBLE_EQ, // ==
  TRIPLE_EQ, // ===
  LESS_THAN,GREATER_THAN,
  LESS_EQ,  GREATER_EQ,
  NUM_TOKEN_TYPES, // counter for enum types
};

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
    std::string stripCommentsAndWhitespace(std::string sourceCode);
};

#endif