#ifndef LEXER_HPP
#define LEXER_HPP

#include "args-parser.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <regex>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

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
  STRING, // [string] -- used for relative path of imports
  COLON, // : -- used for :: in `FullImportedLibrary::aMethodInLib`
  BASE_TYPE, // boolean or integer or floating point types
  KW_TRUE, // true
  KW_FALSE, // false
  DECIMAL_LITERAL,
  // HEX_LITERAL,
  // OCTAL_LITERAL,
  // BINARY_LITERAL,
  FLOAT_LITERAL,
  OPEN_BRACKET,CLOSE_BRACKET, // []
  // ARRAY_DECL, // the [5] in `u8 arr[5]` -- maybe just use open[ + num_lit + close]
  EQUALS,
  PLUS,   MINUS,   STAR,   DIV,   MOD,   BANG, // + - * / % !
  PLUS_EQ,MINUS_EQ,STAR_EQ,DIV_EQ,MOD_EQ,
  KW_STRUCT,KW_UNION,
  PERIOD, // . used for member access -- . in `struct_var.member`
  OPEN_BRACE,CLOSE_BRACE, // {}
  DOUBLE_SLASH, // // used for line comment starting
  OPEN_BLK_CMT, // /*
  CLOSE_BLK_CMT, // */
  OPEN_PAREN,CLOSE_PAREN, // ()
  KW_RETURN,
  KW_IF,
  KW_ELSE,
  KW_WHILE,
  KW_DO,
  KW_BREAK,
  KW_CONTINUE,
  DOUBLE_EQ,     //  ==
  NOT_EQ,        //. !=
  TRIPLE_EQ,     //  ===
  TRIPLE_NOT_EQ, //. !==
  LSS,GTR, // < >
  LEQ,GEQ, // <= >=
  PIPE,DOUBLE_PIPE, // | ||
  AMP,DOUBLE_AMP, // & &&
  APOST,QUOTE,GRAVE, // ' " `
  WHITESPACE,
  COMMENT,
  NUM_TOKEN_TYPES, // counter for enum types
};

std::string typeToString(const TokenType& type);
std::ostream& operator<< (std::ostream& out, const TokenType& type);

class Token {
  public:
    Token(std::string str, TokenType tok_type, int16_t lineNum, int16_t colNum): contents(str), type(tok_type), line(lineNum), col(colNum) {};
    Token(const Token &token): contents(token.contents), type(token.type), line(token.line), col(token.col) {};
    const std::string getContents(void) const {
      return contents;
    }
    const std::string getEscapedContents(void) const {
      return type == TokenType::WHITESPACE || type == TokenType::COMMENT ? std::regex_replace(contents, std::regex{"\\n"}, "\\n") : contents;
    }
    const TokenType getType(void) const {
      return type;
    }
    const int16_t getLine(void) const {
      return line;
    }
    const int16_t getCol(void) const {
      return col;
    }
    std::string toShortString() const; // <type>(<contents>)
    std::string toString() const; // <type> at [line, col]: <contents>
  private:
    const std::string contents;
    const TokenType type;
    const int16_t line;
    const int16_t col;
};

boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const Token& tok);
std::ostream& operator<<(std::ostream& out, const Token& tok);

class Lexer {
  public:
    Lexer(void);
    LexerResult run(std::string sourceCode, std::vector<Token> &resultTokens);
    bool writeTokens(std::vector<Token> &tokens, boost::filesystem::path filePath);
  private:
    bool validateOptionsAndSource(std::string sourceCode);
    std::string preprocessSource(std::string sourceCode);
};

#endif