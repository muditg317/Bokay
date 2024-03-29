#include "lexer.hpp"
#include "args-parser.hpp"

#include <exception>
#include <iostream>
#include <map>
#include <regex>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>
#include <fmt/format.h>
#include <glog/logging.h>

#define MAX_TYPE_STR_LEN "15"

const std::map<TokenType, std::regex> tokenRegexMap {
  { TokenType::WORD, std::regex("(\\b[\\w_][\\w\\d_]*\\b)") },
  { TokenType::SEMICOLON, std::regex("(;)") },
  { TokenType::KW_IMPORT, std::regex("(\\bimport\\b)") },
  { TokenType::COMMA, std::regex("(,)") },
  { TokenType::KW_FROM, std::regex("(\\bfrom\\b)") },
  { TokenType::STRING, std::regex("(\"(\\\\[^\\n]|[^\"\\\\\\n])*\")") }, // ("(\\[^\n]|[^"\\\n])*")
  { TokenType::COLON, std::regex("(:)") },
  { TokenType::BASE_TYPE, std::regex("(\\b((u|s)(8|16|32|64))|(f(32|64))|bool\\b)") },
  { TokenType::KW_TRUE, std::regex("(\\btrue\\b)") },
  { TokenType::KW_FALSE, std::regex("(\\bfalse\\b)") },
  { TokenType::DECIMAL_LITERAL, std::regex("(\\b\\d+\\b)") },
  { TokenType::FLOAT_LITERAL, std::regex("(\\b\\d+(f|\\.\\d+)\\b)") },
  { TokenType::OPEN_BRACKET, std::regex("(\\[)") },
  { TokenType::CLOSE_BRACKET, std::regex("(\\])") },
  { TokenType::EQUALS, std::regex("(=)") },
  { TokenType::PLUS, std::regex("(\\+)") },
  { TokenType::MINUS, std::regex("(-)") },
  { TokenType::STAR, std::regex("(\\*)") },
  { TokenType::DIV, std::regex("(/)") },
  { TokenType::MOD, std::regex("(%)") },
  { TokenType::BANG, std::regex("(!)") },
  { TokenType::PLUS_EQ, std::regex("(\\+=)") },
  { TokenType::MINUS_EQ, std::regex("(-=)") },
  { TokenType::STAR_EQ, std::regex("(\\*=)") },
  { TokenType::DIV_EQ, std::regex("(/=)") },
  { TokenType::MOD_EQ, std::regex("(%=)") },
  { TokenType::KW_STRUCT, std::regex("(\\bstruct\\b)") },
  { TokenType::KW_UNION, std::regex("(\\bunion\\b)") },
  { TokenType::PERIOD, std::regex("(\\.)") },
  { TokenType::OPEN_BRACE, std::regex("(\\{)") },
  { TokenType::CLOSE_BRACE, std::regex("(\\})") },
  { TokenType::DOUBLE_SLASH, std::regex("(//)") },
  { TokenType::OPEN_BLK_CMT, std::regex("(/\\*)") },
  { TokenType::CLOSE_BLK_CMT, std::regex("(\\*/)") },
  { TokenType::OPEN_PAREN, std::regex("(\\()") },
  { TokenType::CLOSE_PAREN, std::regex("(\\))") },
  { TokenType::KW_RETURN, std::regex("(\\breturn\\b)") },
  { TokenType::KW_IF, std::regex("(\\bif\\b)") },
  { TokenType::KW_ELSE, std::regex("(\\belse\\b)") },
  { TokenType::KW_WHILE, std::regex("(\\bwhile\\b)") },
  { TokenType::DOUBLE_EQ, std::regex("(==)") },
  { TokenType::NOT_EQ, std::regex("(!=)") },
  { TokenType::TRIPLE_EQ, std::regex("(===)") },
  { TokenType::TRIPLE_NOT_EQ, std::regex("(!==)") },
  { TokenType::LSS, std::regex("(<)") },
  { TokenType::GTR, std::regex("(>)") },
  { TokenType::LEQ, std::regex("(<=)") },
  { TokenType::GEQ, std::regex("(>=)") },
  { TokenType::PIPE, std::regex("(\\|)") },
  { TokenType::DOUBLE_PIPE, std::regex("(\\|\\|)") },
  { TokenType::AMP, std::regex("(&)") },
  { TokenType::DOUBLE_AMP, std::regex("(&&)") },
  { TokenType::APOST, std::regex("(')") },
  { TokenType::QUOTE, std::regex("(\")") },
  { TokenType::GRAVE, std::regex("(`)") },
  { TokenType::WHITESPACE, std::regex("([^\\S\\n]*\\n?)") },
  { TokenType::COMMENT, std::regex("([^\\S\\n]*((\\/\\/[^\\n]*\\n?)|(\\/\\*(\\*(?!\\/)|[^*]|[\\n])*\\*\\/)))") },
};

std::string typeToString(const TokenType& type) {
  switch(type) {
    case TokenType::WORD: return "WORD";
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::KW_IMPORT: return "KW_IMPORT";
    case TokenType::COMMA: return "COMMA";
    case TokenType::KW_FROM: return "KW_FROM";
    case TokenType::STRING: return "STRING";
    case TokenType::COLON: return "COLON";
    case TokenType::BASE_TYPE: return "BASE_TYPE";
    case TokenType::KW_TRUE: return "KW_TRUE";
    case TokenType::KW_FALSE: return "KW_FALSE";
    case TokenType::DECIMAL_LITERAL: return "DECIMAL_LITERAL";
    case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
    case TokenType::OPEN_BRACKET: return "OPEN_BRACKET";
    case TokenType::CLOSE_BRACKET: return "CLOSE_BRACKET";
    case TokenType::EQUALS: return "EQUALS";
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINUS: return "MINUS";
    case TokenType::STAR: return "STAR";
    case TokenType::DIV: return "DIV";
    case TokenType::MOD: return "MOD";
    case TokenType::BANG: return "BANG";
    case TokenType::PLUS_EQ: return "PLUS_EQ";
    case TokenType::MINUS_EQ: return "MINUS_EQ";
    case TokenType::STAR_EQ: return "STAR_EQ";
    case TokenType::DIV_EQ: return "DIV_EQ";
    case TokenType::MOD_EQ: return "MOD_EQ";
    case TokenType::KW_STRUCT: return "KW_STRUCT";
    case TokenType::KW_UNION: return "KW_UNION";
    case TokenType::PERIOD: return "PERIOD";
    case TokenType::OPEN_BRACE: return "OPEN_BRACE";
    case TokenType::CLOSE_BRACE: return "CLOSE_BRACE";
    case TokenType::DOUBLE_SLASH: return "DOUBLE_SLASH";
    case TokenType::OPEN_BLK_CMT: return "OPEN_BLK_CMT";
    case TokenType::CLOSE_BLK_CMT: return "CLOSE_BLK_CMT";
    case TokenType::OPEN_PAREN: return "OPEN_PAREN";
    case TokenType::CLOSE_PAREN: return "CLOSE_PAREN";
    case TokenType::KW_RETURN: return "KW_RETURN";
    case TokenType::KW_IF: return "KW_IF";
    case TokenType::KW_ELSE: return "KW_ELSE";
    case TokenType::KW_WHILE: return "KW_WHILE";
    case TokenType::DOUBLE_EQ: return "DOUBLE_EQ";
    case TokenType::NOT_EQ: return "NOT_EQ";
    case TokenType::TRIPLE_EQ: return "TRIPLE_EQ";
    case TokenType::TRIPLE_NOT_EQ: return "TRIPLE_NOT_EQ";
    case TokenType::LSS: return "LSS";
    case TokenType::GTR: return "GTR";
    case TokenType::LEQ: return "LEQ";
    case TokenType::GEQ: return "GEQ";
    case TokenType::PIPE: return "PIPE";
    case TokenType::DOUBLE_PIPE: return "DOUBLE_PIPE";
    case TokenType::AMP: return "AMP";
    case TokenType::DOUBLE_AMP: return "DOUBLE_AMP";
    case TokenType::APOST: return "APOST";
    case TokenType::QUOTE: return "QUOTE";
    case TokenType::GRAVE: return "GRAVE";
    case TokenType::WHITESPACE: return "WHITESPACE";
    case TokenType::COMMENT: return "COMMENT";
    default: LOG(ERROR) << "FATAL ERROR: UNKNOWN TOKEN TYPE: " << static_cast<int>(type) ; throw new std::runtime_error("no stringification for token type!");
  }
}

std::ostream& operator<< (std::ostream& out, const TokenType& type) {
  out << typeToString(type);
  return out;
}

std::string Token::toShortString() const {
  return fmt::format(
    "{}(`{}`)",
    typeToString(getType()),
    getEscapedContents());
}
std::string Token::toString() const {
  return fmt::format(
    "{:>" MAX_TYPE_STR_LEN "} at "
    "[Line: {: > 4d}, Column: {: > 3d}]: "
    "`{}`",
    typeToString(getType()),
    getLine(), getCol(),
    getEscapedContents());
}

boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const Token& tok) {
  ofs << tok.toString();
  return ofs;
}

std::ostream& operator<<(std::ostream& out, const Token& tok) {
  out << tok.toString();
  return out;
}

//    (^[^\S\n]*\/\/[^\n]*)|(\/\*(\*(?!\/)|[^*]|[\n])*\*\/)
// const std::regex COMMENT_REGEX("^[^\\S\\n]*((//)|(\\/\\*(\\*(?!\\/)|[^*]|[\\n])*\\*\\/))[^\\n]*$");

//    (\s(?!(?<="((\\[^\n]|[^"\\\n])*))((\\[^\n]|[^"\\\n])*)"))
// const std::regex NON_STRING_SPACE_REGEX

Lexer::Lexer(void) {
  assert(tokenRegexMap.size() == static_cast<uint64_t>(TokenType::NUM_TOKEN_TYPES) && "Must define regex for every token type!");
}

bool Lexer::validateOptionsAndSource(std::string sourceCode) {
  if (sourceCode.empty()) {
    LOG(ERROR) << "Cannot lex empty file!" ;
    return false;
  }
  return true;
}

std::string Lexer::preprocessSource(std::string processed) {
  // these would mess up line numbers! consider adding a token to account for this.
  // processed = std::regex_replace(processed, std::regex{"\\\\[^\\S\\n]*\\n"}, " "); // replace \\n with ` `
  // processed = std::regex_replace(processed, std::regex{"\\n+"}, "\n"); // replace multiple \n with single `\n`
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

  int16_t lineNum = 1;
  int16_t colNum = 1;
  std::match_results<std::string::iterator> match_result;

  while (sourceBegin != sourceEnd) {
    auto index = sourceBegin - processedSource.begin();
    DLOG(INFO) << "Finding token for index: " << index << "\n\tSource: " << processedSource.substr(sourceBegin-processedSource.begin(), 10) ;
    TokenType longestMatchType = TokenType::NUM_TOKEN_TYPES;
    lexing_match longestMatch;
    for (auto tokenRegex : tokenRegexMap) {
      auto regex = tokenRegex.second;
      if (tokenRegex.first == TokenType::NUM_TOKEN_TYPES) {
        LOG(WARNING) << "\tNo matches found, collecting bad token..." ;
        regex = std::regex{"(^\\B+)"};
      }

      bool hasMatch = std::regex_search(
        sourceBegin, sourceEnd, // match starting at beginning of unmatched portion
        match_result, regex, // save result based on applied regex
        std::regex_constants::match_continuous); // ensure match starts at `sourceBegin`
      if (!hasMatch) { // no match_result found for this regex
        DLOG(INFO) << "\tNo match for type: " << tokenRegex.first ;
        continue;
      }
      lexing_match match = match_result[0];
      DLOG(INFO) << "\tFound match! Type: " << tokenRegex.first ;
      size_t matchLen = match.length();
      if (tokenRegex.first == TokenType::NUM_TOKEN_TYPES) {
        LOG(ERROR) << "Invalid token found!\n\tLocation: [Line: " << lineNum << ", Column: " << colNum << "]\n\tToken: " << match.str() << "\n";
        return LexerResult::INVALID_TOKENS;
      }
      // TODO: must be >= because WORD is lowest priority since KW_* is more important than WORD
      if (matchLen >= longestMatch.length()) {
        longestMatchType = tokenRegex.first;
        longestMatch = match;
      }
    }
    if (longestMatchType == TokenType::NUM_TOKEN_TYPES) {
      LOG(ERROR) << "FATAL LEXING ERROR!\n";
      LOG(ERROR) <<
        "Failed token parsing at index: " << index <<
        "\n\tSource: " << processedSource.substr(index, 10) <<
        "\n\t        ^";
      return LexerResult::INVALID_TOKENS;
    }

    resultTokens.push_back(Token{longestMatch.str(), longestMatchType, lineNum, colNum});
    DLOG(INFO) << "Register token: " << resultTokens.back() ;
    sourceBegin += longestMatch.length();

    // update linenum and colnum based on captured token
    bool linesAdded = false;
    if (longestMatchType == TokenType::WHITESPACE || longestMatchType == TokenType::COMMENT) {
      // std::smatch match_result;
      std::regex_search(longestMatch.first, longestMatch.second, match_result, std::regex{"\\n"});
      size_t extraLines = match_result.size();
      if (extraLines) { // newlines found, add lineNum and update colNum
        DLOG(INFO) << "New line found in whitespace/comment!\n";
        DLOG(INFO) << "\t" << resultTokens.back() ;
        DLOG(INFO) << "\t" << extraLines ;
        linesAdded = true;
        lineNum += extraLines;
        colNum = 1 + longestMatch.second - match_result[extraLines-1].second; // end of match - end of last newline (start at 1)
      }
    }
    if (!linesAdded) { // no new lines, increment colNum as needed
      colNum += longestMatch.length();
    }
  }

  return LexerResult::LEXING_SUCCESS;
}

bool Lexer::writeTokens(std::vector<Token> &tokens, boost::filesystem::path filePath) {
  if (!boost::filesystem::exists(filePath.parent_path())) {
    LOG(ERROR) << "Cannot write tokens to " << filePath << " because parent dir does not exist!" ;
    return false;
  }

  boost::filesystem::ofstream tokenFile{filePath};

  for (auto token : tokens) {
    tokenFile << token << '\n';
  }

  tokenFile.close();
  return true;
}