#ifndef PARSER_HPP
#define PARSER_HPP

#include "lexer.hpp"

#include <algorithm>
#include <queue>
#include <vector>
#include <variant>

#include <boost/filesystem/path.hpp>


enum class ParserResult {
  PARSING_SUCCESS,
  INVALID_TOKEN_FOUND,
  FAILED_RECOGNITION,
};

enum class ParseNodeType {
  // RAW_TOKEN,
  // KW_IMPORT,KW_FROM,
  // STRING,
  SOURCE, // [imports] [statements]
  SOURCE_BODY, // [imports] [statements]
  IMPORT_GROUP, // [import_group] [import_stmnt] | [import_stmnt]
  IMPORT_STATEMENT, // import WholeLib, {method, otherField} from "./lib.bokay";
  LIB_ACCESSOR, // ::
  ID, // variable
  LITERAL, // [raw_lit] | [minus] [raw_lit]
  RAW_LITERAL, // decimal | float
  TERM, // ( [expression] )  | [var_use] | [func call]
  VARIABLE_USE, // [id] | [var_use][arr_access]
  ARRAY_ACCESS, // [ [expression] ]
  EXPRESSION, // [expression] [operator] [term] | [term]
  ARITHMETIC_EXPRESSION, // [expression] [operator] [term] | [term]
  ARITHMETIC_OPERATOR, // [+-]
  MULTIPLICATIVE_EXPRESSION, // [expression] [operator] [term] | [term]
  MULTIPLICATIVE_OPERATOR, // [*/%]
  UNARY_EXPRESSION, // [operator] [expression]
  UNARY_OPERATOR, // [!-]
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
std::string typeToString(const ParseNodeType& type);
std::ostream& operator<< (std::ostream& out, const ParseNodeType& type);

#define PARSE_NODE_TYPE_INDEX 0
#define TOKEN_TYPE_INDEX 1
typedef std::variant<ParseNodeType, TokenType> RuleComponent;
typedef std::variant<ParseNodeType, Token> ParseNode;

std::ostream& operator<< (std::ostream& out, const ParseNode& node);

/**
 * @brief represents a single production rule
 *  the rhs/body of the rule
 */
struct Production {
  const std::vector<RuleComponent> components;
  Production(const std::vector<RuleComponent> _components): components(_components) {};
  size_t length() const {return components.size();};
  RuleComponent operator[](size_t n) const {return components[n];};
  bool operator==(const Production other) const {return components == other.components;};
};
std::ostream& operator<< (std::ostream& out, const Production& production);

struct ParsingTree;
typedef std::variant<ParseNode, ParsingTree> ParseTreeChild;

struct ParsingTree {
  ParseNode root;
  std::vector<ParseTreeChild> children;
  public:
    ParsingTree(): root(Token{"",TokenType::NUM_TOKEN_TYPES,-1,-1}) {};
    ParsingTree(ParseNode rootType): root(rootType) {};
    std::string toTabbedString(void) const;
    // ParsingTree &operator=(const ParsingTree &) = default;
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
  bool operator==(ParsingState &other) const {
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


class Parser {
  public:
    Parser(void);
    ParserResult run(std::vector<Token> tokens, ParseTree *&resultTree) const;
    bool writeTree(ParseTree &ptree, boost::filesystem::path filePath);
  private:
    bool prediction(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd) const; // run the predictor on this state
    bool scanning(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd, std::vector<Token> &tokens) const; // run the scanner on this state
    bool completion(std::vector<ParsingStateSet> &stateSets, ParsingState &state, size_t tokInd) const; // run the completer on this state
};

#endif