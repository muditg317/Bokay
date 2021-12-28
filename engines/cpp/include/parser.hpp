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
  EXPRESSION, // [stuff] operator [stuff]
  OPERATOR, // +-*/%
  DECLARATION, // [base type] [variable_use]
  VARIABLE_USE, // [id] | [var_use][arr_access]
  ARRAY_ACCESS, // [ expression ]
  ASSIGNMENT, // [LHS] equals [expression]
  STATEMENTS, // [statements] [statement] | [statement]
  STATEMENT, // [anything];
  // STRUCT_DEF, UNION_DEF,
  // CONDITIONALS, LOOPS,
  // FUNCTIONS
  USELESS, // [whitespace] | [comment]
  NUM_NODE_TYPES
};

struct ProductionBase {
  protected:
    ProductionBase() {};
};

typedef std::variant<ParseNodeType, TokenType> RuleComponent;

// std::__detal

/**
 * @brief represents a single production rule
 * @tparam T the lhs/head of the rule
 */
template <ParseNodeType T>
struct Production : ProductionBase {
  /**
   * @brief the rhs/body of the rule
   */
  const std::vector<RuleComponent> components;
  Production(const std::vector<RuleComponent> _components): components(_components) {};
};

class ParsingTree {
  RuleComponent root;
  std::vector<ParsingTree> children;
};

struct ParsingStateBase {
  uint8_t numMatchedComponents;
  const size_t matchOrigin;
  ParsingTree matchedTree;
  ParsingStateBase() = delete;
  protected:
    ParsingStateBase(size_t _matchOrigin): numMatchedComponents(0), matchOrigin(_matchOrigin) {};
};

template<ParseNodeType T>
struct ParsingState : ParsingStateBase {
  Production<T> currentProduction;
  ParsingState(Production<T> _currentProduction, size_t origin): ParsingStateBase(origin), currentProduction(_currentProduction) {};
};

struct ParsingStateSet {
  std::deque<ParsingStateBase> states;
  bool addState(ParsingStateBase); // checks for duplicates before adding the new state to the internal queue
};


class Parser {
  public:
    Parser(void);
    ParserResult run(std::vector<Token> tokens);
  private:
    std::vector<ParsingStateSet> stateSets;
    bool prediction(ParsingStateBase &state, uint16_t tokInd); // run the predictor on this state
};

#endif