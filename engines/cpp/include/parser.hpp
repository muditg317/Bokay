#ifndef PARSER_HPP
#define PARSER_HPP

#include <array>

enum class ParserResult {
  PARSING_SUCCESS,
  INVALID_TOKEN_FOUND,
  // INVALID_SYNTAX,
};

enum class ParseNodeType {
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
  USELESS, // [whitespace] | [comment]
  NUM_NODE_COUNT
};

class GrammarRuleUnion;
class GrammarRuleConcatenation;

class GrammarRule {
  public:
    GrammarRule(ParseNodeType type);
    GrammarRule(std::array<std::array<ParseNodeType> ruleInfo): rule(std::) {};
  private:
    GrammarRuleUnion rule;
};

class GrammarRuleUnion {
  public:
    GrammarRuleUnion(GrammarRuleConcatenation &rule): options(std::array<GrammarRuleConcatenation>{rule}) {};
    GrammarRuleUnion(std::array<GrammarRuleConcatenation> &ruleOptions): options(ruleOptions) {};
  private:
    std::array<GrammarRuleConcatenation> options;
};

class GrammarRuleConcatentation {
  public:
    GrammarRuleConcatenation(ParseNodeType &node): parseNodes(std::array{node}) {};
    GrammarRuleConcatenation(std::array<ParseNodeType> &nodeSeries): parseNodes(nodeSeries) {};
  private:
    std::array<ParseNodeType> parseNodes;
};

#endif