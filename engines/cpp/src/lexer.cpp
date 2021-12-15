#include "lexer.hpp"
#include "args-parser.hpp"

#include <iostream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

Lexer::Lexer(Options &options) {
  
}

bool Lexer::validateOptionsAndSource(std::string sourceCode) {
  if (sourceCode.empty()) {
    std::cout << "Cannot lex empty file!" << std::endl;
    return false;
  }
  return true;
}

LexerResult Lexer::run(std::string sourceCode, std::vector<Token> &resultTokens) {
  if (!validateOptionsAndSource(sourceCode)) {
    return INVALID_LEXING_OPTIONS;
  }

  auto splitString = boost::algorithm::split(*new std::vector<std::string>(), sourceCode, boost::algorithm::is_space());

  resultTokens.push_back(Token(splitString.at(0)));

  return LEXING_SUCCESS;
}