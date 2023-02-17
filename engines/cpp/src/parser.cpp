#include "parser.hpp"
#include "grammar.hpp"
#include "lexer.hpp"

#include <algorithm>
#include <map>
// #include <pair>
#include <ostream>
#include <typeinfo>
#include <vector>

#include <boost/filesystem.hpp>
#include <glog/logging.h>



std::string ParsingTree::toTabbedString(void) const {
  std::ostringstream oss{"\n"};
  std::deque<std::pair<ParseTreeChild,size_t>> treeNodeQueue{}; // node,depth queue
  treeNodeQueue.push_front(std::make_pair(*this, 0));
  while (!treeNodeQueue.empty()) {
    ParseTreeChild node = treeNodeQueue.front().first;
    size_t depth = treeNodeQueue.front().second;
    treeNodeQueue.pop_front();
    if (node.index() == PARSE_TREE_CHILD_TYPE_LEAF) { // node is a leaf
      oss << std::string(depth, '\t') << std::get<ParseNode>(node) << '\n';
      continue;
    }
    ParsingTree subtree = std::get<ParsingTree>(node);

    if (subtree.isLeaf()) {
      oss << std::string(depth, '\t') << subtree.root << " - " << std::get<ParseNode>(subtree.children[0]) << '\n';
    } else if (subtree.isDegenerateTree()) {
      oss << std::string(depth, '\t') << subtree.root;
      ParsingTree descendant_tree =
          std::get<ParsingTree>(subtree.children[0])
            .recursivelyPrintDegenerateSubtrees(oss, " > ");
      if (descendant_tree.isLeaf()) {
        oss << " - " << std::get<ParseNode>(descendant_tree.children[0])<< '\n';
      } else {
        oss << " > " << descendant_tree.root << '\n';
        for (std::vector<ParseTreeChild>::reverse_iterator i = descendant_tree.children.rbegin(); 
            i != descendant_tree.children.rend(); ++i ) {
          treeNodeQueue.push_front(std::make_pair(*i, depth+1));
        }
      }
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

ParsingTree ParsingTree::recursivelyPrintDegenerateSubtrees(std::ostream &stream, std::string prefix) const {
  if (children.size() > 1) {
    return *this;
  }
  if (isLeaf() || childTreeIsLeaf()) {
    return *this;
  }
  stream << prefix << root;
  return std::get<ParsingTree>(children[0]).recursivelyPrintDegenerateSubtrees(stream, prefix);
}



Parser::Parser(void) {
  CHECK(grammarRuleMap.size() == static_cast<size_t>(ParseNodeType::NUM_NODE_TYPES)) << "Must define rule for every parse node type!" << std::endl << "Rule types: " << static_cast<int>(ParseNodeType::NUM_NODE_TYPES) << std::endl << "Rule map size: " << grammarRuleMap.size();
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

  ParsingState *sourceRootParseState = nullptr;
  bool fullyMatched = false;

  DLOG(INFO) << "Last state states:";
  for (size_t i = 0; i < stateSets.back().size(); i++) {
    auto &state = stateSets.back().states[i];
    DLOG(INFO) << "\t" << state.ruleType << "[" << state.matchOrigin << "] - " << state.currentProduction;
    DLOG(INFO) << "\t\tCompleted: " << state.numMatchedComponents << "/" << state.currentProduction.length();
    DLOG(INFO) << "\t----";
    if (state.ruleType == ParseNodeType::SOURCE) {
      sourceRootParseState = &state;
      if (state.isFullyMatched()) {
        fullyMatched = true;
        break; // accept first one because other ones may be unmatched
      }
    }
  }

  if (!fullyMatched) {
    LOG(ERROR) << "Parsing failed to recognize source from grammar!";
    // TODO: provide info about longest match seen or something
    #ifdef DEBUG
    DLOG(INFO) << "Last state states:";
    for (size_t i = 0; i < stateSets.back().size(); i++) {
      auto &state = stateSets.back().states[i];
      DLOG(INFO) << "\t" << state.ruleType << "[" << state.matchOrigin << "] - " << state.currentProduction;
      DLOG(INFO) << "\t\tCompleted: " << state.numMatchedComponents << "/" << state.currentProduction.length();
      DLOG(INFO) << "\t----";
    }
    #endif
    if (sourceRootParseState) {
      ParsingTree finalTree = sourceRootParseState->matchedTree;
      LOG(INFO) << "Printing final tree:\n" << finalTree.toTabbedString();
    #ifdef DEBUG
    } else {
      DLOG(INFO) << "No source root parse state found!";
    #endif
    }
    return ParserResult::FAILED_RECOGNITION;
  }

  // if reached: parsing finished successfully!
  DLOG(INFO) << "Parsing succeeded! Printing tree...";
  ParsingTree finalTree = sourceRootParseState->matchedTree;
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
  if (tokInd >= tokens.size()) {
    // LOG(ERROR) << "Token index out of bounds!";
    // assert(tokInd < tokens.size() && "Token index out of bounds!");
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

  ptreeFile.close();
  return true;
}