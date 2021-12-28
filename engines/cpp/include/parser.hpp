#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

#include <algorithm>
#include <vector>

enum class ParserResult {
  PARSING_SUCCESS,
  INVALID_TOKEN_FOUND,
  // INVALID_SYNTAX,
};

enum class ParseNodeType {
  KW_IMPORT,
  KW_FROM,
  KW_STRUCT,
  KW_UNION,
  KW_RETURN,
  KW_IF,
  KW_WHILE,
  STRING,
  IMPORT_STATEMENT, // import WholeLib, {method, otherField} from "./lib.bokay";
  LIB_ACCESSOR, // ::
  ID, // variable
  EXPRESSION, // [stuff] operator [stuff]
  OPERATOR, // +-*/%
  DECLARATION, // [base type] [variable_use]
  VARIABLE_USE, // [id] | [var_use][arr_access]
  ARRAY_ACCESS, // [ expression ]
  ASSIGNMENT, // [LHS] equals [expression]
  STATEMENT, // [anything];
  // STRUCT_DEF, UNION_DEF,
  // CONDITIONALS, LOOPS,
  // FUNCTIONS
  USELESS, // [whitespace] | [comment]
  NUM_NODE_TYPES
};

class GrammarRuleUnion;

class GrammarRuleConcatenation {
  public:
    GrammarRuleConcatenation(const std::vector<GrammarRuleUnion> &_components): components(_components) {};
  private:
    const std::vector<GrammarRuleUnion> components;
};

class GrammarRuleUnion {
  public:
    GrammarRuleUnion(const std::vector<GrammarRuleConcatenation> &_options): options(_options) {};
  private:
    const std::vector<GrammarRuleConcatenation> options;
};


class Parser {
  public:
    Parser(void);
    ParserResult run(std::vector<Token> tokens);
};

#endif