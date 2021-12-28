#include "parser.hpp"
#include "lexer.hpp"

#include <map>
#include <vector>

const std::map<ParseNodeType, GrammarRuleUnion> grammarRuleMap {
  { ParseNodeType::IMPORT_STATEMENT, GrammarRuleUnion{
    GrammarRuleConcatenation{
      ParseNodeType::KW_IMPORT,
      GrammarRuleUnion{ParseNodeType::ID,
      },
      ParseNodeType::KW_FROM,
      ParseNodeType::STRING,
    }
  } },
};

Parser::Parser(void) {
  assert(grammarRuleMap.size() == ParseNodeType::NUM_NODE_TYPES && "Must define rule for every parse node type!");
}

ParserResult run(std::vector<Token> tokens) {
  return ParserResult::PARSING_SUCCESS;
}