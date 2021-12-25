#include "lexer.hpp"
#include "args-parser.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <regex>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

const std::map<TokenType, std::regex> tokenRegexMap {
  { TokenType::WORD, std::regex("(\\b[\\w_][\\w\\d_]*\\b)") },
  { TokenType::SEMICOLON, std::regex("(;)") },
  { TokenType::KW_IMPORT, std::regex("(\\bimport\\b)") },
  { TokenType::COMMA, std::regex("(,)") },
  { TokenType::KW_FROM, std::regex("(\\bfrom\\b)") },
  { TokenType::IMPORT_SOURCE, std::regex("(\"(\\\\[^\\n]|[^\"\\\\\\n])*\")") }, // ("(\\[^\n]|[^"\\\n])*")
  { TokenType::COLON, std::regex("(::)") },
  { TokenType::BASE_TYPE, std::regex("(\\b((u|s)(8|16|32|64))|(f(32|64))\\b)") },
  { TokenType::DECIMAL_LITERAL, std::regex("(\\b-?\\d+\\b)") },
  { TokenType::FLOATING_POINT_LITERAL, std::regex("(\\b(-?\\d+)f|-?\\d+\\.\\d+\\b)") },
  { TokenType::OPEN_BRACKET, std::regex("(\\[)") },
  { TokenType::CLOSE_BRACKET, std::regex("(\\])") },
  { TokenType::EQUALS, std::regex("(=)") },
  { TokenType::PLUS, std::regex("(\\+)") },
  { TokenType::MINUS, std::regex("(-)") },
  { TokenType::MULT, std::regex("(\\*)") },
  { TokenType::DIV, std::regex("(/)") },
  { TokenType::MOD, std::regex("(%)") },
  { TokenType::PLUS_EQ, std::regex("(\\+=)") },
  { TokenType::MINUS_EQ, std::regex("(-=)") },
  { TokenType::MULT_EQ, std::regex("(\\*=)") },
  { TokenType::DIV_EQ, std::regex("(/=)") },
  { TokenType::MOD_EQ, std::regex("(%=)") },
  { TokenType::KW_STRUCT, std::regex("(\\bstruct\\b)") },
  { TokenType::KW_UNION, std::regex("(\\bunion\\b)") },
  { TokenType::PERIOD, std::regex("(\\.)") },
  { TokenType::OPEN_BRACE, std::regex("(\\{)") },
  { TokenType::CLOSE_BRACE, std::regex("(\\})") },
  { TokenType::LINE_COMMENT, std::regex("(//)") },
  { TokenType::OPEN_BLK_CMT, std::regex("(/\\*)") },
  { TokenType::CLOSE_BLK_CMT, std::regex("(\\*/)") },
  { TokenType::OPEN_PAREN, std::regex("(\\()") },
  { TokenType::CLOSE_PAREN, std::regex("(\\))") },
  { TokenType::KW_RETURN, std::regex("(\\breturn\\b)") },
  { TokenType::KW_IF, std::regex("(\\bif\\b)") },
  { TokenType::KW_WHILE, std::regex("(\\bwhile\\b)") },
  { TokenType::DOUBLE_EQ, std::regex("(==)") },
  { TokenType::TRIPLE_EQ, std::regex("(===)") },
  { TokenType::LSS, std::regex("(<)") },
  { TokenType::GTR, std::regex("(>)") },
  { TokenType::LEQ, std::regex("(<=)") },
  { TokenType::GEQ, std::regex("(>=)") },
  { TokenType::WHITESPACE, std::regex("(\\s+)") },
};

std::ostream& operator<< (std::ostream& out, const TokenType& type) {
  switch(type) {
    case TokenType::WORD: out << "WORD"; break;
    case TokenType::SEMICOLON: out << "SEMICOLON"; break;
    case TokenType::KW_IMPORT: out << "KW_IMPORT"; break;
    case TokenType::COMMA: out << "COMMA"; break;
    case TokenType::KW_FROM: out << "KW_FROM"; break;
    case TokenType::IMPORT_SOURCE: out << "IMPORT_SOURCE"; break;
    case TokenType::COLON: out << "COLON"; break;
    case TokenType::BASE_TYPE: out << "BASE_TYPE"; break;
    case TokenType::DECIMAL_LITERAL: out << "DECIMAL_LITERAL"; break;
    case TokenType::FLOATING_POINT_LITERAL: out << "FLOATING_POINT_LITERAL"; break;
    case TokenType::OPEN_BRACKET: out << "OPEN_BRACKET"; break;
    case TokenType::CLOSE_BRACKET: out << "CLOSE_BRACKET"; break;
    case TokenType::EQUALS: out << "EQUALS"; break;
    case TokenType::PLUS: out << "PLUS"; break;
    case TokenType::MINUS: out << "MINUS"; break;
    case TokenType::MULT: out << "MULT"; break;
    case TokenType::DIV: out << "DIV"; break;
    case TokenType::MOD: out << "MOD"; break;
    case TokenType::PLUS_EQ: out << "PLUS_EQ"; break;
    case TokenType::MINUS_EQ: out << "MINUS_EQ"; break;
    case TokenType::MULT_EQ: out << "MULT_EQ"; break;
    case TokenType::DIV_EQ: out << "DIV_EQ"; break;
    case TokenType::MOD_EQ: out << "MOD_EQ"; break;
    case TokenType::KW_STRUCT: out << "KW_STRUCT"; break;
    case TokenType::KW_UNION: out << "KW_UNION"; break;
    case TokenType::PERIOD: out << "PERIOD"; break;
    case TokenType::OPEN_BRACE: out << "OPEN_BRACE"; break;
    case TokenType::CLOSE_BRACE: out << "CLOSE_BRACE"; break;
    case TokenType::LINE_COMMENT: out << "LINE_COMMENT"; break;
    case TokenType::OPEN_BLK_CMT: out << "OPEN_BLK_CMT"; break;
    case TokenType::CLOSE_BLK_CMT: out << "CLOSE_BLK_CMT"; break;
    case TokenType::OPEN_PAREN: out << "OPEN_PAREN"; break;
    case TokenType::CLOSE_PAREN: out << "CLOSE_PAREN"; break;
    case TokenType::KW_RETURN: out << "KW_RETURN"; break;
    case TokenType::KW_IF: out << "KW_IF"; break;
    case TokenType::KW_WHILE: out << "KW_WHILE"; break;
    case TokenType::DOUBLE_EQ: out << "DOUBLE_EQ"; break;
    case TokenType::TRIPLE_EQ: out << "TRIPLE_EQ"; break;
    case TokenType::LSS: out << "LSS"; break;
    case TokenType::GTR: out << "GTR"; break;
    case TokenType::LEQ: out << "LEQ"; break;
    case TokenType::GEQ: out << "GEQ"; break;
    case TokenType::WHITESPACE: out << "WHITESPACE"; break;
    default: out << "FATAL ERROR: UNKNOWN TOKEN TYPE: " << static_cast<int>(type) << std::endl; throw new std::runtime_error("no stringification for token type!");
  }
  return out;
}

//    (^[^\S\n]*\/\/[^\n]*)|(\/\*(\*(?!\/)|[^*]|[\n])*\*\/)
// const std::regex COMMENT_REGEX("^[^\\S\\n]*((//)|(\\/\\*(\\*(?!\\/)|[^*]|[\\n])*\\*\\/))[^\\n]*$");

//    (\s(?!(?<="((\\[^\n]|[^"\\\n])*))((\\[^\n]|[^"\\\n])*)"))
// const std::regex NON_STRING_SPACE_REGEX

Lexer::Lexer(Options &options) {
  assert(tokenRegexMap.size() == TokenType::NUM_TOKEN_TYPES && "Must define regex for every token type!");
}

//

bool Lexer::validateOptionsAndSource(std::string sourceCode) {
  if (sourceCode.empty()) {
    std::cout << "Cannot lex empty file!" << std::endl;
    return false;
  }
  return true;
}

std::string Lexer::preprocessSource(std::string processed) {
  processed = std::regex_replace(processed, std::regex{"\\\\[^\\S\\n]*\\n"}, " "); // replace \\n with ` `
  processed = std::regex_replace(processed, std::regex{"\\n+"}, "\n"); // replace multiple \n with single `\n`
  return processed;
}

typedef std::match_results<std::string::iterator>::value_type lexing_match;

LexerResult Lexer::run(std::string sourceCode, std::vector<Token> &resultTokens) {
  if (!validateOptionsAndSource(sourceCode)) {
    return LexerResult::INVALID_LEXING_OPTIONS;
  }

  std::string processedSource = preprocessSource(sourceCode);

  auto sourceBegin = processedSource.begin();
  auto sourceEnd = processedSource.end();

  while (sourceBegin != sourceEnd) {
    // std::cout << "Finding token for index: " << sourceBegin - processedSource.begin() << "\n\tSource: " << processedSource.substr(sourceBegin-processedSource.begin(), 10) << std::endl;
    TokenType longestMatchType = TokenType::NUM_TOKEN_TYPES;
    lexing_match longestMatch;
    for (auto tokenRegex : tokenRegexMap) {
      auto regex = tokenRegex.second;
      if (tokenRegex.first == TokenType::NUM_TOKEN_TYPES) {
        std::cout << "\tNo matches found, collecting bad token..." << std::endl;
        regex = std::regex{"(^\\B+)"};
      }
      std::match_results<std::string::iterator> match_result;

      bool hasMatch = std::regex_search(
        sourceBegin, sourceEnd, // match starting at beginning of unmatched portion
        match_result, regex, // save result based on applied regex
        std::regex_constants::match_continuous); // ensure match starts at `sourceBegin`
      if (!hasMatch) { // no match_result found for this regex
        // std::cout << "\tNo match for type: " << tokenRegex.first << std::endl;
        continue;
      }
      lexing_match match = match_result[0];
      // std::cout << "\tFound match! Type: " << tokenRegex.first << std::endl;
      size_t matchLen = match.length();
      if (tokenRegex.first == TokenType::NUM_TOKEN_TYPES) {
        std::cout << "Invalid token found!\n\tLocation: " << sourceBegin-processedSource.begin() << "\n\tToken: " << match.str() << "\n";
        return LexerResult::INVALID_TOKENS;
      }
      if (matchLen > longestMatch.length()) {
        longestMatchType = tokenRegex.first;
        longestMatch = match;
      }
    }
    if (longestMatchType == TokenType::NUM_TOKEN_TYPES) {
      std::cout << "FATAL LEXING ERROR!\n";
      return LexerResult::INVALID_TOKENS;
    }
    // std::cout << "Register token{" << longestMatchType << "}: `" << longestMatch.str() << "`" << std::endl;
    resultTokens.push_back(Token{longestMatch.str(), longestMatchType});
    sourceBegin += longestMatch.length();
  }

  return LexerResult::LEXING_SUCCESS;
}