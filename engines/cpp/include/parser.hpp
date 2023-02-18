#pragma once

#include "compiler-stage.hpp"
#include "lexer.hpp"
#include "grammar.hpp"

#include <algorithm>
#include <ostream>
#include <queue>
#include <vector>
#include <variant>

#include <boost/filesystem/path.hpp>


enum class ParserResult {
  PARSING_SUCCESS,
  INVALID_TOKEN_FOUND,
  FAILED_RECOGNITION,
};

struct ParsingTree;

enum ParseTreeChildType {
  PARSE_TREE_CHILD_TYPE_LEAF = 0,
  PARSE_TREE_CHILD_TYPE_SUBTREE = 1,
};
typedef std::variant<ParseNode, ParsingTree> ParseTreeChild;

struct ParsingTree {
  ParseNode root;
  std::vector<ParseTreeChild> children;
 public:
  ParsingTree(): root(Token{"",TokenType::NUM_TOKEN_TYPES,-1,-1}) {};
  ParsingTree(ParseNode rootType): root(rootType) {};
  std::string toTabbedString(void) const;
  /**
   * @brief print a degenerate section of the tree in a single line
   * 
   * @param stream where to print the tree nodes to
   * @param prefix the prefix for the tree node names
   * @return ParsingTree the lowest descendant of the tree that is not degenerate or a leaf
   */
  ParsingTree recursivelyPrintDegenerateSubtrees(std::ostream &stream, std::string prefix) const;
  // ParsingTree &operator=(const ParsingTree &) = default;

  inline bool hasOneChild(void) const {return children.size() == 1;};

  inline bool isLeaf(void) const {
    return hasOneChild() && std::holds_alternative<ParseNode>(children[0]);
  };

  inline bool isDegenerateTree(void) const {
    return hasOneChild() && std::holds_alternative<ParsingTree>(children[0]);
  };

  inline bool childTreeIsLeaf(void) const {
    return isDegenerateTree() && std::get<ParsingTree>(children[0]).isLeaf();
  };
};

// struct ParseTree {
//   ParseNode root;
//   std::vector<ParseTreeChild> children;
//   public:
//     ParseTree() {};
//     void setFrom(ParsingTree &tree);
// };
typedef ParsingTree ParseTree;


struct ParsingState {
  const ParseNodeType ruleType;
  const Production currentProduction;
  size_t numMatchedComponents;
  const size_t matchOrigin;
  ParsingTree matchedTree;
  ParsingState() = delete;
  // protected:
  ParsingState(ParseNodeType _ruleType, Production _currentProduction, size_t _matchOrigin): ruleType(_ruleType), currentProduction(_currentProduction), numMatchedComponents(0), matchOrigin(_matchOrigin), matchedTree(_ruleType) {};
  bool isFullyMatched() {return numMatchedComponents == currentProduction.length();};
  RuleComponent nextUnmatchedComponent() const {return currentProduction[numMatchedComponents];};
  ParseNodeType nextUnmatchedAsNonTerminal() const {return std::get<ParseNodeType>(nextUnmatchedComponent());};
  TokenType nextUnmatchedAsTerminal() const {return std::get<TokenType>(nextUnmatchedComponent());};
  bool nextComponentIsTerminal() const {return nextUnmatchedComponent().index() == TOKEN_TYPE_INDEX;};
  bool operator==(const ParsingState &other) const {
    if (ruleType != other.ruleType) return false;
    if (!(currentProduction == other.currentProduction)) return false;
    if (matchOrigin != other.matchOrigin) return false;
    return true;
    // return ruleType == other.ruleType && currentProduction == other.currentProduction && matchOrigin == other.matchOrigin;
  };
  ParsingState advanced(ParseTreeChild) const;
};

struct ParsingStateSet {
  ParsingStateSet();

  std::deque<ParsingState> states;
  bool addState(ParsingState newState, bool force = false); // checks for duplicates before adding the new state to the internal queue
  size_t size() const {return states.size();};
};

// static auto ParserName = make_string("Parser");
// static auto ParserTmpOutExt = make_string("ptree");
using ParserBase = CompilerStage<std::vector<Token>, ParseTree, ParserResult, "Parser", "ptree">;
class Parser : public ParserBase {
 public:
  using Base = ParserBase;

  Parser(void);
  Base::ErrorType operator()(Base::InputType &rawTokens, Base::OutputType *&resultTree) const override;
  bool writeOutput(Base::OutputType &ptree, boost::filesystem::path filePath) const override;
  void debugCallback(Base::OutputType &ptree) const override;
 private:
  bool prediction(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd) const; // run the predictor on this state
  bool scanning(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd, std::vector<Token> &tokens) const; // run the scanner on this state
  bool completion(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd) const; // run the completer on this state
};