#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

#include <algorithm>
#include <queue>
#include <vector>
#include <variant>

enum class ParserResult {
  PARSING_SUCCESS,
  INVALID_TOKEN_FOUND,
  // INVALID_SYNTAX,
};

enum class ParseNodeType {
  // RAW_TOKEN,
  // KW_IMPORT,KW_FROM,
  // STRING,
  SOURCE, // [imports] [statements]
  IMPORT_GROUP, // [import_group] [import_stmnt] | [import_stmnt]
  IMPORT_STATEMENT, // import WholeLib, {method, otherField} from "./lib.bokay";
  LIB_ACCESSOR, // ::
  ID, // variable
  TERM, // ( [expression] )  | [var_use] | [func call]
  VARIABLE_USE, // [id] | [var_use][arr_access]
  ARRAY_ACCESS, // [ [expression] ]
  EXPRESSION, // [expression] [operator] [term] | [term]
  OPERATOR, // +-*/%
  DECLARATION, // [base type] [variable_use]
  LHS, // [decl] | [var_use]
  ASSIGNMENT, // [LHS] equals [expression]
  STATEMENTS, // [statements] [statement] | [statement]
  STATEMENT_BODY, // [anything]
  STATEMENT, // [stmt_body] ;
  // STRUCT_DEF, UNION_DEF,
  // CONDITIONALS, LOOPS,
  // FUNCTIONS
  // USELESS, // [whitespace] | [comment]
  NUM_NODE_TYPES
};

#define PARSE_NODE_TYPE_INDEX 0
#define TOKEN_TYPE_INDEX 1
typedef std::variant<ParseNodeType, TokenType> RuleComponent;
typedef std::variant<ParseNodeType, Token> ParseNode;

/**
 * @brief represents a single production rule
 * @tparam T the lhs/head of the rule
 */
struct Production {
  /**
   * @brief the rhs/body of the rule
   */
  const std::vector<RuleComponent> components;
  Production(const std::vector<RuleComponent> _components): components(_components) {};
  size_t length() const {return components.size();};
  RuleComponent operator[](size_t n) const {return components[n];};
  bool operator==(const Production other) const {return components == other.components;};
};

struct ParsingTree;
typedef std::variant<ParseNode, ParsingTree> ParseTreeChild;

struct ParsingTree {
  ParseNode root;
  std::vector<ParseTreeChild> children;
  public:
    ParsingTree(ParseNode rootType): root(rootType) {};
};

struct ParsingState {
  const ParseNodeType ruleType;
  const Production currentProduction;
  uint8_t numMatchedComponents;
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
  bool operator==(ParsingState other) const {return currentProduction == other.currentProduction && matchOrigin == other.matchOrigin;};
  ParsingState advanced(ParseTreeChild) const;
};

struct ParsingStateSet {
  std::deque<ParsingState> states;
  bool addState(ParsingState); // checks for duplicates before adding the new state to the internal queue
  size_t size() const {return states.size();};
};


class Parser {
  public:
    Parser(void);
    ParserResult run(std::vector<Token> tokens) const;
  private:
    bool prediction(std::vector<ParsingStateSet> &stateSets, ParsingState &state, uint16_t tokInd) const; // run the predictor on this state
    bool scanning(std::vector<ParsingStateSet> &stateSets, ParsingState &state, uint16_t tokInd, std::vector<Token> &tokens) const; // run the scanner on this state
    bool completion(std::vector<ParsingStateSet> &stateSets, ParsingState &state, uint16_t tokInd) const; // run the completer on this state
};

#endif