#include "parser.hpp"
#include "lexer.hpp"

#include <map>
#include <vector>

#include <glog/logging.h>



const std::map<const ParseNodeType, const std::vector<ProductionBase>> grammarRuleMap {
  { ParseNodeType::SOURCE, {
    Production<ParseNodeType::SOURCE>{ {
      ParseNodeType::IMPORT_GROUP,
      ParseNodeType::STATEMENTS,
    } }
  } },
  { ParseNodeType::IMPORT_STATEMENT, {
    Production<ParseNodeType::IMPORT_STATEMENT>{ {
      TokenType::KW_IMPORT,
      ParseNodeType::ID,
      TokenType::KW_FROM,
      TokenType::STRING,
    } }
  } },
};

template<ParseNodeType T>
Production<T> getRule(int i = 0) {
  return *static_cast<const Production<T> *>(&grammarRuleMap.at(T)[i]);
}

Parser::Parser(void) {
  // CHECK()
  // LOG(INFO) << "grammar rules size: " << grammarRuleMap.size();
  // LOG(INFO) << "num parse types: " << static_cast<int>(ParseNodeType::NUM_NODE_TYPES);
  // CHECK(grammarRuleMap.size() == static_cast<int>(ParseNodeType::NUM_NODE_TYPES) && "Must define rule for every parse node type!");
}

ParserResult Parser::run(std::vector<Token> tokens) {
  // One stateSet per gap between tokens (and beginning+end)
  stateSets.reserve(tokens.size() + 1);
  stateSets.clear();
  stateSets[0].states.push_back(ParsingState<ParseNodeType::SOURCE>{getRule<ParseNodeType::SOURCE>(), 0});

  for (uint16_t tokenIndex = 0, tokensSize = tokens.size(); tokenIndex <= tokensSize; tokenIndex++) {
    for (auto &state : stateSets[tokenIndex].states) {
      // if (state.numMatchedComponents < state.)
      prediction(state, tokenIndex);
    }
  }

  return ParserResult::PARSING_SUCCESS;
}

bool Parser::prediction(ParsingStateBase &state, uint16_t tokInd) {

}