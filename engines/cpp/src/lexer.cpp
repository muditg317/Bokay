#include "lexer.hpp"
#include "args-parser.hpp"

#include <iostream>
#include <regex>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

const std::regex tokenRegex[(size_t) TokenType::NUM_TOKEN_TYPES] = {
  std::regex("([\\w_][\\w\\d_]*)"), //  NAME
  std::regex("(;)"), //  LINE_ENDING
  std::regex("(import)"), //  KW_IMPORT
  std::regex("(,)"), //  IMPORT_DELIM
  std::regex("(from)"), //  KW_FROM
  std::regex("(\"(\\\\[^\\n]|[^\"\\\\\\n])*\")"), //  IMPORT_SOURCE -- ("(\\[^\n]|[^"\\\n])*")
  std::regex("(::)"), //  LIB_ACCESSOR
  std::regex("(((u|s)(8|16|32|64))|(f(32|64)))"), //  BASE_TYPE
  std::regex("(-?\\d+)"), //  DECIMAL_LITERAL
  std::regex("((-?\\d+)f|-?\\d+\\.\\d+)"), //  FLOATING_POINT_LITERAL
  std::regex("(\\[)"), //  OPEN_BRACKET
  std::regex("(\\])"), //  CLOSE_BRACKET
  std::regex("(=)"), //  EQUALS
  std::regex("(\\+)"), //  PLUS
  std::regex("(-)"), //  MINUS
  std::regex("(\\*)"), //  MULT
  std::regex("(/)"), //  DIV
  std::regex("(%)"), //  MOD
  std::regex("(\\+=)"), //  PLUS_EQ
  std::regex("(-=)"), //  MINUS_EQ
  std::regex("(\\*=)"), //  MULT_EQ
  std::regex("(/=)"), //  DIV_EQ
  std::regex("(%=)"), //  MOD_EQ
  std::regex("(struct)"), //  KW_STRUCT
  std::regex("(union)"), //  KW_UNION
  std::regex("(\\.)"), //  MEMBER_ACCESSOR
  std::regex("(\\{)"), //  OPEN_BRACE
  std::regex("(\\})"), //  CLOSE_BRACE
  // std::regex("(//)"), //  COMMENT_STARTER
  std::regex("(\\()"), //  OPEN_PAREN
  std::regex("(\\))"), //  CLOSE_PAREN
  std::regex("(return)"), //  KW_RETURN
  std::regex("(if)"), //  KW_IF
  std::regex("(while)"), //  KW_WHILE
  std::regex("(==)"), //  DOUBLE_EQ
  std::regex("(===)"), //  TRIPLE_EQ
  std::regex("(<)"), //  LESS_THAN
  std::regex("(>)"), //  GREATER_THAN
  std::regex("(<=)"), //  LESS_EQ
  std::regex("(>=)"), //  GREATER_EQ
};

//    (^[^\S\n]*\/\/[^\n]*)|(\/\*(\*(?!\/)|[^*]|[\n])*\*\/)
const std::regex COMMENT_REGEX("^[^\\S\\n]*((//)|(\\/\\*(\\*(?!\\/)|[^*]|[\\n])*\\*\\/))[^\\n]*$");

//    (\s(?!(?<="((\\[^\n]|[^"\\\n])*))((\\[^\n]|[^"\\\n])*)"))
// const std::regex NON_STRING_SPACE_REGEX

Lexer::Lexer(Options &options) {
  
}

//

bool Lexer::validateOptionsAndSource(std::string sourceCode) {
  if (sourceCode.empty()) {
    std::cout << "Cannot lex empty file!" << std::endl;
    return false;
  }
  return true;
}

std::string Lexer::stripCommentsAndWhitespace(std::string stripped) {
  stripped = std::regex_replace(stripped, COMMENT_REGEX, "");
  stripped = std::regex_replace(stripped, COMMENT_REGEX, "");
  return stripped;
}

LexerResult Lexer::run(std::string sourceCode, std::vector<Token> &resultTokens) {
  if (!validateOptionsAndSource(sourceCode)) {
    return LexerResult::INVALID_LEXING_OPTIONS;
  }

  std::string strippedSource = stripCommentsAndWhitespace(sourceCode);


  auto splitString = boost::algorithm::split(*new std::vector<std::string>(), sourceCode, boost::algorithm::is_space());

  resultTokens.push_back(Token(splitString.at(0), TokenType::NAME));

  return LexerResult::LEXING_SUCCESS;
}