#include "parser.hpp"
#include "lexer.hpp"

#include <algorithm>
#include <map>
#include <typeinfo>
#include <vector>

#include <glog/logging.h>



const std::map<const ParseNodeType, const std::vector<Production>> grammarRuleMap {
  { ParseNodeType::SOURCE, {
    Production{ {
      ParseNodeType::IMPORT_GROUP,
      ParseNodeType::STATEMENTS,
    } },
  } },
  { ParseNodeType::IMPORT_GROUP, {
    Production{ {
      ParseNodeType::IMPORT_GROUP,
      ParseNodeType::IMPORT_STATEMENT,
    } },
    Production{ {
      ParseNodeType::IMPORT_STATEMENT,
    } },
  } },
  { ParseNodeType::IMPORT_STATEMENT, {
    Production{ {
      TokenType::KW_IMPORT,
      ParseNodeType::ID,
      TokenType::KW_FROM,
      TokenType::STRING,
    } },
  } },
  { ParseNodeType::LIB_ACCESSOR, {
    Production{ {
      TokenType::COLON,
      TokenType::COLON,
    } },
  } },
  { ParseNodeType::ID, {
    Production{ {
      TokenType::WORD,
    } },
  } },
  { ParseNodeType::TERM, {
    Production{ {
      TokenType::OPEN_PAREN,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_PAREN,
    } },
    Production{ {
      ParseNodeType::VARIABLE_USE,
    } },
    // Production{ {
    //   TokenType::FUNCTION_CALL,
    // } },
  } },
  { ParseNodeType::VARIABLE_USE, {
    Production{ {
      ParseNodeType::VARIABLE_USE,
      ParseNodeType::ARRAY_ACCESS,
    } },
    Production{ {
      TokenType::ID,
    } },
  } },
  { ParseNodeType::ARRAY_ACCESS, {
    Production{ {
      TokenType::OPEN_BRACKET,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_BRACKET,
    } },
  } },
  { ParseNodeType::EXPRESSION, {
    Production{ {
      ParseNodeType::EXPRESSION,
      ParseNodeType::OPERATOR,
      ParseNodeType::TERM,
    } },
    Production{ {
      ParseNodeType::TERM,
    } },
  } },
  { ParseNodeType::OPERATOR, {
    Production{ {
      TokenType::PLUS,
    } },
    Production{ {
      TokenType::MINUS,
    } },
    Production{ {
      TokenType::MULT,
    } },
    Production{ {
      TokenType::DIV,
    } },
  } },
  { ParseNodeType::DECLARATION, {
    Production{ {
      TokenType::BASE_TYPE,
      ParseNodeType::VARIABLE_USE,
    } },
  } },
  { ParseNodeType::LHS, {
    Production{ {
      ParseNodeType::DECLARATION,
    } },
    Production{ {
      ParseNodeType::VARIABLE_USE,
    } },
  } },
  { ParseNodeType::ASSIGNMENT, {
    Production{ {
      ParseNodeType::LHS,
      TokenType::EQUALS,
      ParseNodeType::EXPRESSION,
    } },
  } },
  { ParseNodeType::STATEMENTS, {
    Production{ {
      ParseNodeType::STATEMENTS,
      ParseNodeType::STATEMENT,
    } },
    Production{ {
      ParseNodeType::STATEMENT,
    } },
  } },
  { ParseNodeType::STATEMENT_BODY, {
    Production{ {
      ParseNodeType::DECLARATION,
    } },
    Production{ {
      ParseNodeType::ASSIGNMENT,
    } },
  } },
  { ParseNodeType::STATEMENT, {
    Production{ {
      ParseNodeType::STATEMENT_BODY,
      TokenType::SEMICOLON,
    } },
  } },
};

inline std::vector<Production> getRules(ParseNodeType type) {
  return grammarRuleMap.at(type);
}
inline Production getRule(ParseNodeType type, int i = 0) {
  return grammarRuleMap.at(type)[i];
}

Parser::Parser(void) {
  // CHECK()
  // LOG(INFO) << "grammar rules size: " << grammarRuleMap.size();
  // LOG(INFO) << "num parse types: " << static_cast<int>(ParseNodeType::NUM_NODE_TYPES);
  CHECK(grammarRuleMap.size() == static_cast<int>(ParseNodeType::NUM_NODE_TYPES)) << "Must define rule for every parse node type!";
}

bool uselessToken(Token tok) {
  return tok.getType() == TokenType::WHITESPACE || tok.getType() == TokenType::COMMENT;
}
std::vector<Token> &filteredTokens(std::vector<Token> &rawTokens) {
  // std::vector<Token> *tokens = new std::vector<Token>();
  // std::remove_copy_if(rawTokens.begin(), rawTokens.end(), tokens->front(), uselessToken);
  // return *tokens;
  return rawTokens;
}

ParserResult Parser::run(std::vector<Token> rawTokens) const {
  std::vector<Token> tokens = filteredTokens(rawTokens);

  // One stateSet per gap between tokens (and beginning+end)
  std::vector<ParsingStateSet> stateSets{tokens.size() + 1};
  // stateSets.clear();
  stateSets[0].states.push_back(ParsingState{ParseNodeType::SOURCE, getRule(ParseNodeType::SOURCE), 0});

  for (uint16_t tokenIndex = 0, tokensSize = tokens.size(); tokenIndex <= tokensSize; tokenIndex++) {
    bool stateChanged = false;
    for (uint16_t stateIndex = 0; stateIndex < stateSets[tokenIndex].size(); stateIndex++) {
    // for (auto &state : stateSets[tokenIndex].states) {
      auto &state = stateSets[tokenIndex].states[stateIndex];
      if (state.isFullyMatched()) { // the rule has been full matched
        stateChanged |= completion(stateSets, state, tokenIndex);
        continue;
      }
      // if reached: the rule is not completely matched
      RuleComponent component = state.currentProduction.components[state.numMatchedComponents]; // get the first unmatched component in the rule
      // component is either ParseNodeType (non-terminal) or TokenType (terminal)
      if (!state.nextComponentIsTerminal()) { // component is non-terminal
        stateChanged |= prediction(stateSets, state, tokenIndex);
        continue;
      }
      // if reached: component is terminal
      stateChanged |= scanning(stateSets, state, tokenIndex, tokens);
    }
  }

  return ParserResult::PARSING_SUCCESS;
}

bool Parser::prediction(std::vector<ParsingStateSet> &stateSets, ParsingState &state, uint16_t tokInd) const {
  ParseNodeType nextExpectedNonTerminal = state.nextUnmatchedAsNonTerminal();
  bool added = false;
  for (Production rule : getRules(nextExpectedNonTerminal)) {
    added |= stateSets[tokInd].addState(ParsingState{nextExpectedNonTerminal, rule, tokInd});
  }
  return added;
}

bool Parser::scanning(std::vector<ParsingStateSet> &stateSets, ParsingState &state, uint16_t tokInd, std::vector<Token> &tokens) const {
  TokenType nextExpectedTerminal = state.nextUnmatchedAsTerminal();
  Token nextToken = tokens[tokInd+1];
  if (nextToken.getType() == nextExpectedTerminal) {
    stateSets[tokInd+1].addState(state.advanced(nextToken));
    return true;
  }
  return false;
}

bool Parser::completion(std::vector<ParsingStateSet> &stateSets, ParsingState &state, uint16_t tokInd) const {
  bool added = false;
  for (auto originState : stateSets[state.matchOrigin].states) { // for each state in the origin of this match
    if (!originState.isFullyMatched() && !originState.nextComponentIsTerminal()) { // origin state has a non-terminal next
      if (originState.nextUnmatchedAsNonTerminal() == state.ruleType) { // the next expected non-terminal finished here
        stateSets[tokInd].addState(originState.advanced(state.matchedTree));
        added = true;
      }
    }
  }
  return added;
}

// add state only if unique
bool ParsingStateSet::addState(ParsingState newState) {
  for (auto existing : states) {
    if (existing == newState) {
      return false;
    }
  }
  states.push_back(newState);
  return true;
}

ParsingState ParsingState::advanced(ParseTreeChild recognizedChild) const {
  ParsingState newState = ParsingState{ruleType, currentProduction, matchOrigin};
  newState.numMatchedComponents = numMatchedComponents + 1;
  std::for_each(matchedTree.children.begin(), matchedTree.children.end(), [&newState](auto treeChild) {
    newState.matchedTree.children.push_back(treeChild);
  });
  newState.matchedTree.children.push_back(recognizedChild);
  return newState;
}