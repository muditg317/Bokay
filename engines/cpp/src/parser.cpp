#include "parser.hpp"
#include "lexer.hpp"

#include <algorithm>
#include <map>
// #include <pair>
#include <typeinfo>
#include <vector>

#include <boost/filesystem.hpp>
#include <glog/logging.h>


const std::map<const ParseNodeType, const std::vector<Production>> grammarRuleMap {
  { ParseNodeType::SOURCE, {
    Production{ {
      ParseNodeType::SOURCE_BODY,
    } },
  } },
  { ParseNodeType::SOURCE_BODY, {
    Production{ {
      ParseNodeType::IMPORT_GROUP,
      ParseNodeType::STATEMENTS,
    } },
    Production{ {
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
  { ParseNodeType::LITERAL, {
    Production{ {
      ParseNodeType::RAW_LITERAL,
    } },
    Production{ {
      TokenType::MINUS,
      ParseNodeType::RAW_LITERAL,
    } },
  } },
  { ParseNodeType::RAW_LITERAL, {
    Production{ {
      TokenType::DECIMAL_LITERAL,
    } },
    Production{ {
      TokenType::FLOAT_LITERAL,
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
    Production{ {
      ParseNodeType::LITERAL,
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
      ParseNodeType::ID,
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
      TokenType::STAR,
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

std::string typeToString(const ParseNodeType& type) {
  switch(type) {
    case ParseNodeType::SOURCE: return "SOURCE";
    case ParseNodeType::SOURCE_BODY: return "SOURCE_BODY";
    case ParseNodeType::IMPORT_GROUP: return "IMPORT_GROUP";
    case ParseNodeType::IMPORT_STATEMENT: return "IMPORT_STATEMENT";
    case ParseNodeType::LIB_ACCESSOR: return "LIB_ACCESSOR";
    case ParseNodeType::ID: return "ID";
    case ParseNodeType::LITERAL: return "LITERAL";
    case ParseNodeType::RAW_LITERAL: return "RAW_LITERAL";
    case ParseNodeType::TERM: return "TERM";
    case ParseNodeType::VARIABLE_USE: return "VARIABLE_USE";
    case ParseNodeType::ARRAY_ACCESS: return "ARRAY_ACCESS";
    case ParseNodeType::EXPRESSION: return "EXPRESSION";
    case ParseNodeType::OPERATOR: return "OPERATOR";
    case ParseNodeType::DECLARATION: return "DECLARATION";
    case ParseNodeType::LHS: return "LHS";
    case ParseNodeType::ASSIGNMENT: return "ASSIGNMENT";
    case ParseNodeType::STATEMENTS: return "STATEMENTS";
    case ParseNodeType::STATEMENT_BODY: return "STATEMENT_BODY";
    case ParseNodeType::STATEMENT: return "STATEMENT";
    default: LOG(ERROR) << "FATAL ERROR: UNKNOWN PARSE NODE TYPE: " << static_cast<int>(type) ; throw new std::runtime_error("no stringification for parse node type!");
  }
}
std::ostream& operator<< (std::ostream& out, const ParseNodeType& type) {
  out << typeToString(type);
  return out;
}

std::ostream& operator<< (std::ostream& out, const Production& production) {
  std::for_each(production.components.begin(), production.components.end(), [&out, &production](RuleComponent component) {
    if (component.index() == PARSE_NODE_TYPE_INDEX) {
      out << std::get<ParseNodeType>(component);
    } else {
      out << std::get<TokenType>(component);
    }
    // if (component != production.components.end().operator*()) {
      out << " ";//+ ";
    // }
  });
  return out;
}

std::ostream& operator<< (std::ostream& out, const ParseNode& node) {
  if (node.index() == PARSE_NODE_TYPE_INDEX) {
    out
      // << "ParseNode{" 
      << std::get<ParseNodeType>(node);
  } else {
    out 
      // << "Token{"
      << std::get<Token>(node).toShortString();
  }
  // out << "}";
  return out;
}


std::string ParsingTree::toTabbedString(void) const {
  std::ostringstream oss{"\n"};
  std::deque<std::pair<ParseTreeChild,size_t>> treeNodeQueue{}; // node,depth queue
  treeNodeQueue.push_front(std::make_pair(*this, 0));
  while (!treeNodeQueue.empty()) {
    ParseTreeChild node = treeNodeQueue.front().first;
    size_t depth = treeNodeQueue.front().second;
    treeNodeQueue.pop_front();
    if (node.index() == 0) { // node is a leaf
      oss << std::string(depth, '\t') << std::get<ParseNode>(node) << '\n';
      continue;
    }
    ParsingTree subtree = std::get<ParsingTree>(node);

    if (subtree.children.size() == 1 && subtree.children[0].index() == 0) {
      oss << std::string(depth, '\t') << subtree.root << " - " << std::get<ParseNode>(subtree.children[0])<< '\n';
    } else {
      oss << std::string(depth, '\t') << subtree.root << '\n';
      for (std::vector<ParseTreeChild>::reverse_iterator i = subtree.children.rbegin(); 
          i != subtree.children.rend(); ++i ) {
        treeNodeQueue.push_front(std::make_pair(*i, depth+1));
      }
    }
  }
  return oss.str();
}



Parser::Parser(void) {
  CHECK(grammarRuleMap.size() == static_cast<int>(ParseNodeType::NUM_NODE_TYPES)) << "Must define rule for every parse node type!";
}

bool uselessToken(Token tok) {
  return tok.getType() == TokenType::WHITESPACE || tok.getType() == TokenType::COMMENT;
}
void filterTokens(std::vector<Token> &rawTokens, std::vector<Token> &outTokens) {
  std::for_each(rawTokens.begin(), rawTokens.end(), [&outTokens](Token rawTok) {
    if (!uselessToken(rawTok)) {
      outTokens.push_back(rawTok);
    }
  });
}

ParserResult Parser::run(std::vector<Token> rawTokens, ParseTree *&resultTree) const {
  std::vector<Token> tokens;
  filterTokens(rawTokens, tokens);

  if (tokens.size() == 0) {
    LOG(ERROR) << "Source has no usefull tokens!";
    return ParserResult::FAILED_RECOGNITION;
  }

  DLOG(INFO) << "Begin parsing with " << tokens.size() << " tokens";
  std::ostringstream oss;
  std::for_each(tokens.begin(), tokens.end(), [&oss](Token tok) {
    oss << tok.getType() << ", ";
  });
  DLOG(INFO) << "Tokens: " << oss.str();

  // One stateSet per gap between tokens (and beginning+end)
  std::vector<ParsingStateSet> stateSets{tokens.size() + 1};
  // stateSets.clear();
  stateSets[0].states.push_back(ParsingState{ParseNodeType::SOURCE, getRule(ParseNodeType::SOURCE), 0});


  for (size_t tokenIndex = 0, tokensSize = tokens.size(); tokenIndex <= tokensSize; tokenIndex++) {
    DLOG(INFO) << "Parse for tokenIndex=" << tokenIndex;
    DLOG_IF(INFO, tokenIndex < tokensSize) << "\t" << tokens[tokenIndex].toShortString();
    DLOG_IF(INFO, tokenIndex == tokensSize) << "\tfinal stateSet";
    bool stateChanged = false;
    for (size_t stateIndex = 0; stateIndex < stateSets[tokenIndex].size(); stateIndex++) {
      DLOG(INFO) << "Parse for stateIndex=" << stateIndex << "(tokInd=" << tokenIndex << ")";
      auto &state = stateSets[tokenIndex].states[stateIndex];
      DLOG(INFO) << "\t" << state.ruleType << "[" << state.matchOrigin << "] - " << state.currentProduction;
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

  DLOG(INFO) << "Parsed statesSets (" << stateSets.size() << " sets) ---";
  for (size_t i = 0; i < stateSets.size(); i++) {
    auto set = stateSets[i];
    if (set.size() == 0) continue;
    DLOG(INFO) << "Set " << i << " is not empty!";
    DLOG(INFO) << "\tSet size: " << set.size() << "|";
    auto &last = set.states.front();
    DLOG(INFO) << "\tnum: " << last.numMatchedComponents << "|";
    DLOG(INFO) << "\torigin: " << last.matchOrigin << "|";
    DLOG(INFO) << "\trule: " << last.ruleType;
    DLOG(INFO) << "----";
  }

  ParsingState *successfulParseState = nullptr;

  DLOG(INFO) << "Last state states:";
  for (size_t i = 0; i < stateSets.back().size(); i++) {
    auto &state = stateSets.back().states[i];
    DLOG(INFO) << "\t" << state.ruleType << "[" << state.matchOrigin << "] - " << state.currentProduction;
    DLOG(INFO) << "\t\tCompleted: " << state.numMatchedComponents << "/" << state.currentProduction.length();
    DLOG(INFO) << "\t----";
    if (state.ruleType == ParseNodeType::SOURCE && state.isFullyMatched()) {
      successfulParseState = &state;
      break; // accept first one because other ones may be unmatched
    }
  }

  if (!successfulParseState) {
    LOG(ERROR) << "Parsing failed to recognize source from grammar!";
    // TODO: provide info about longest match seen or something
    return ParserResult::FAILED_RECOGNITION;
  }

  // if reached: parsing finished successfully!
  DLOG(INFO) << "Parsing succeeded! Printing tree...";
  ParsingTree finalTree = successfulParseState->matchedTree;
  std::deque<std::pair<ParseTreeChild,size_t>> treeNodeQueue{}; // node,depth queue
  treeNodeQueue.push_front(std::make_pair(finalTree, 0));
  while (!treeNodeQueue.empty()) {
    ParseTreeChild node = treeNodeQueue.front().first;
    size_t depth = treeNodeQueue.front().second;
    treeNodeQueue.pop_front();
    if (node.index() == 0) { // node is a leaf
      DLOG(INFO) << "Leaf        (d=" << depth << "):\t" << std::get<ParseNode>(node);
      // LOG(INFO) << std::string(depth, '\t') << std::get<ParseNode>(node);
      continue;
    }
    ParsingTree subtree = std::get<ParsingTree>(node);

    if (subtree.children.size() == 1 && subtree.children[0].index() == 0) {
      DLOG(INFO) << "Pseudo-leaf (d=" << depth << "):\t" << subtree.root << " - " << std::get<ParseNode>(subtree.children[0]);
      // LOG(INFO) << std::string(depth, '\t') << subtree.root << " - " << std::get<ParseNode>(subtree.children[0]);
    } else {
      // LOG(INFO) << std::string(depth, '\t') << subtree.root;
      DLOG(INFO) << "(sub)root: " << subtree.root;
      DLOG(INFO) << subtree.children.size() << " children";
      for (std::vector<ParseTreeChild>::reverse_iterator i = subtree.children.rbegin(); 
          i != subtree.children.rend(); ++i ) {
        treeNodeQueue.push_front(std::make_pair(*i, depth+1));
      }
    }
  }
  resultTree = new ParseTree(finalTree.root);

  // resultTree = ParseTree{finalTree.root};
  // if (finalTree.root.index() == PARSE_NODE_TYPE_INDEX) { // ParseNodeType
  //   resultTree.root = std::get<ParseNodeType>(finalTree.root);
  // } else {
  //   Token &resultRoot = std::get<Token>(resultTree.root);
  //   resultRoot = std::get<Token>(finalTree.root);
  //   // root = Token{std::get<Token>(tree.root)};
  // }
  for (auto child : finalTree.children) {
    resultTree->children.push_back(child);
  }

  return ParserResult::PARSING_SUCCESS;
}

bool Parser::prediction(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd) const {
  DLOG(INFO) << "PREDICTION " << state.ruleType;
  // DLOG(INFO) << "\t\tfrom index=" << tokInd;
  // DLOG(INFO) << "\t\ton potential " << state.ruleType;
  // DLOG(INFO) << "\t\toriginating from " << state.matchOrigin;
  ParseNodeType nextExpectedNonTerminal = state.nextUnmatchedAsNonTerminal();
  DLOG(INFO) << "\tExpecting " << nextExpectedNonTerminal;
  bool added = false;
  for (Production rule : getRules(nextExpectedNonTerminal)) {
    bool newRule = stateSets[tokInd].addState(ParsingState{nextExpectedNonTerminal, rule, tokInd});
    added |= newRule;
    DLOG_IF(INFO, newRule) << "\t\tAdd rule: " << rule;
  }
  DLOG_IF(INFO, !added) << "\t\tNo rules added";
  return added;
}

bool Parser::scanning(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd, std::vector<Token> &tokens) const {
  assert(tokInd < tokens.size() && "Token index out of bounds!");
  if (tokInd >= tokens.size()) {
    // LOG(ERROR) << "Token index out of bounds!";
    return false;
  }
  DLOG(INFO) << "SCANNING " << state.ruleType;
  TokenType nextExpectedTerminal = state.nextUnmatchedAsTerminal();
  DLOG(INFO) << "\tExpecting:\t" << nextExpectedTerminal;
  Token nextToken = tokens[tokInd];
  DLOG(INFO) << "\tNext token:\t" << nextToken.getType();
  if (nextToken.getType() == nextExpectedTerminal) {
    DLOG(INFO) << "" << nextToken;
    assert(stateSets.size() > tokInd+1 && "Token index out of bounds");
    // DLOG(INFO) << "Token " << nextToken << " matched " << state.ruleType << " at index " << tokInd;
    // DLOG(INFO) << "Adding to stateSets size: " << stateSets.size() << " - index: " << (tokInd+1);
    stateSets[tokInd+1].addState(state.advanced(nextToken));
    return true;
  } else {
    DLOG(INFO) << "\t\tScan failed";
  }
  return false;
}

bool Parser::completion(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd) const {
  DLOG(INFO) << "COMPLETION " << state.ruleType;
  bool added = false;
  for (auto originState : stateSets[state.matchOrigin].states) { // for each state in the origin of this match
    if (!originState.isFullyMatched() && !originState.nextComponentIsTerminal()) { // origin state has a non-terminal next
      if (originState.nextUnmatchedAsNonTerminal() == state.ruleType) { // the next expected non-terminal finished here
        DLOG(INFO) << "\tPull rule: " << originState.ruleType << "[" << originState.matchOrigin << "]";
        ParsingState newState = originState.advanced(state.matchedTree);
        bool newStateAdded = stateSets[tokInd].addState(newState, true);
        // DLOG_IF(INFO, originState.ruleType == ParseNodeType::EXPRESSION)
        //   << "\t" << (newStateAdded ? "" : "FAILED TO ") << "Pull expression from: " << originState.matchOrigin
        //   << "\n\texpr=" << originState.currentProduction
        //   << "\n" << newState.matchedTree.toTabbedString();
        added |= newStateAdded;
      }
    }
  }
  return added;
}

ParsingStateSet::ParsingStateSet() {
  states.clear();
}

// add state only if unique
bool ParsingStateSet::addState(ParsingState newState, bool force) {
  if (!force) {
    bool already_exists = std::any_of(states.begin(), states.end(), [&](ParsingState &existing) {
      // DLOG(INFO) << "\t\tFailed to add new state: " << newState.ruleType << "=" << newState.currentProduction << "|new-" << newState.numMatchedComponents << " vs old-" << existing.numMatchedComponents;
      return existing == newState;
    });
    if (already_exists) {
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

// void ParseTree::setFrom(ParsingTree &tree) {
//   if (tree.root.index() == PARSE_NODE_TYPE_INDEX) { // ParseNodeType
//     root = std::get<ParseNodeType>(tree.root);
//   } else {
//     // root = Token{std::get<Token>(tree.root)};
//   }
//   std::for_each(tree.children.begin(), tree.children.end(), [this](ParseTreeChild child) {
//     this->children.push_back(child);
//   });
// };

bool Parser::writeTree(ParseTree &ptree, boost::filesystem::path filePath) {
  if (!boost::filesystem::exists(filePath.parent_path())) {
    LOG(ERROR) << "Cannot write ptree to " << filePath << " because parent dir does not exist!" ;
    return false;
  }

  boost::filesystem::ofstream ptreeFile{filePath};

  ptreeFile << ptree.toTabbedString();

  // for (auto token : tokens) {
  //   ptreeFile << token << '\n';
  // }

  ptreeFile.close();
  return true;
}