#include "grammar.hpp"

#include <stdexcept>
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
      ParseNodeType::IMPORT_CONTENTS,
      TokenType::KW_FROM,
      TokenType::STRING,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::IMPORT_CONTENTS, {
    Production{ {
      ParseNodeType::IMPORT_LIB_NAME,
    } },
    Production{ {
      ParseNodeType::IMPORT_ITEM_GROUP,
    } },
    Production{ {
      ParseNodeType::IMPORT_LIB_NAME,
      TokenType::COMMA,
      ParseNodeType::IMPORT_ITEM_GROUP,
    } },
  } },
  { ParseNodeType::IMPORT_LIB_NAME, {
    Production{ {
      ParseNodeType::IMPORT_ITEM,
    } },
  } },
  { ParseNodeType::IMPORT_ITEM_GROUP, {
    Production{ {
      TokenType::OPEN_BRACE,
      ParseNodeType::IMPORT_ITEMS,
      TokenType::CLOSE_BRACE,
    } },
  } },
  { ParseNodeType::IMPORT_ITEMS, {
    Production{ {
      ParseNodeType::IMPORT_ITEMS,
      TokenType::COMMA,
      ParseNodeType::IMPORT_ITEM,
    } },
    Production{ {
      ParseNodeType::IMPORT_ITEM,
    } },
  } },
  { ParseNodeType::IMPORT_ITEM, {
    Production{ {
      ParseNodeType::ID,
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
  { ParseNodeType::STATEMENT, {
    Production{ {
      ParseNodeType::STATEMENT_BODY,
      TokenType::SEMICOLON,
    } },
    Production{ {
      ParseNodeType::FUNCTION_IMPL,
    } },
    Production{ {
      ParseNodeType::RETURN_STMT,
    } },
    Production{ {
      ParseNodeType::CONDITION_CHAIN,
    } },
    Production{ {
      ParseNodeType::LOOP,
    } },
    Production{ {
      ParseNodeType::STRUCT_DECL,
    } },
    Production{ {
      ParseNodeType::UNION_DECL,
    } },
  } },
  { ParseNodeType::STATEMENT_BODY, {
    Production{ {
      ParseNodeType::DECLARATION,
    } },
    Production{ {
      ParseNodeType::EXPRESSION,
    } },
  } },
  { ParseNodeType::CODE_BLOCK, {
    Production{ {
      TokenType::OPEN_BRACE,
      ParseNodeType::STATEMENTS,
      TokenType::CLOSE_BRACE,
    } },
  } },
  { ParseNodeType::DECLARATION, {
    Production{ {
      ParseNodeType::DECL_SPECIFIER,
      ParseNodeType::DECL_LIST,
    } },
  } },
  { ParseNodeType::DECL_SPECIFIER, {
    Production{ {
      TokenType::BASE_TYPE,
    } },
    Production{ {
      ParseNodeType::ID,
    } },
  } },
  { ParseNodeType::DECL_LIST, {
    Production{ {
      ParseNodeType::DECL_LIST,
      TokenType::COMMA,
      ParseNodeType::DECL_ITEM,
    } },
    Production{ {
      ParseNodeType::DECL_ITEM,
    } },
  } },
  { ParseNodeType::DECL_ITEM, {
    Production{ {
      ParseNodeType::DECLARATOR,
    } },
    Production{ {
      ParseNodeType::DECLARATOR,
      TokenType::EQUALS,
      ParseNodeType::INITIALIZER,
    } },
  } },
  { ParseNodeType::INITIALIZER, {
    Production{ {
      ParseNodeType::EXPRESSION,
    } },
  } },
  { ParseNodeType::DECLARATOR, {
    Production{ {
      ParseNodeType::ID,
    } },
    Production{ {
      ParseNodeType::PAREN_DECL,
    } },
    Production{ {
      ParseNodeType::PTR_DECL,
    } },
    Production{ {
      ParseNodeType::ARRAY_DECL,
    } },
    Production{ {
      ParseNodeType::FUNC_DECL,
    } },
  } },
  { ParseNodeType::PAREN_DECL, {
    Production{ {
      TokenType::OPEN_PAREN,
      ParseNodeType::DECLARATOR,
      TokenType::CLOSE_PAREN,
    } },
  } },
  { ParseNodeType::PTR_DECL, {
    Production{ {
      TokenType::STAR,
      ParseNodeType::DECLARATOR,
    } },
  } },
  { ParseNodeType::NO_PTR_DECL, {
    Production{ {
      ParseNodeType::ID,
    } },
    Production{ {
      ParseNodeType::PAREN_DECL,
    } },
    Production{ {
      ParseNodeType::ARRAY_DECL,
    } },
    Production{ {
      ParseNodeType::FUNC_DECL,
    } },
  } },
  { ParseNodeType::ARRAY_DECL, {
    Production{ {
      ParseNodeType::NO_PTR_DECL,
      TokenType::OPEN_BRACKET,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_BRACKET,
    } },
  } },
  { ParseNodeType::FUNC_DECL, {
    Production{ {
      ParseNodeType::NO_PTR_DECL,
      TokenType::OPEN_PAREN,
      ParseNodeType::PARAM_LIST,
      TokenType::CLOSE_PAREN,
    } },
  } },
  { ParseNodeType::PARAM_LIST, {
    Production{ {
      ParseNodeType::PARAM_LIST,
      TokenType::COMMA,
      ParseNodeType::PARAM_ITEM,
    } },
    Production{ {
      ParseNodeType::PARAM_ITEM,
    } },
  } },
  { ParseNodeType::PARAM_ITEM, {
    Production{ {
      ParseNodeType::FUNC_TYPE_DECL,
    } },
  } },
  { ParseNodeType::FUNC_TYPE_DECL, {
    Production{ {
      ParseNodeType::DECL_SPECIFIER,
      ParseNodeType::DECLARATOR,
    } },
  } },
  { ParseNodeType::FUNCTION_IMPL, {
    Production{ {
      ParseNodeType::FUNC_TYPE_DECL,
      TokenType::OPEN_PAREN,
      ParseNodeType::FUNCTION_ARGUMENTS,
      TokenType::CLOSE_PAREN,
      ParseNodeType::CODE_BLOCK,
    } },
  } },
  { ParseNodeType::FUNCTION_ARGUMENTS, {
    Production{ {
      ParseNodeType::FUNCTION_ARGUMENTS,
      TokenType::COMMA,
      ParseNodeType::FUNCTION_ARGUMENT,
    } },
    Production{ {
      ParseNodeType::FUNCTION_ARGUMENT,
    } },
  } },
  { ParseNodeType::FUNCTION_ARGUMENT, {
    Production{ {
      ParseNodeType::FUNC_TYPE_DECL,
      TokenType::EQUALS,
      ParseNodeType::DEFAULT_FUNC_ARG_VALUE,
    } },
    Production{ {
      ParseNodeType::FUNC_TYPE_DECL,
    } },
  } },
  { ParseNodeType::DEFAULT_FUNC_ARG_VALUE, {
    Production{ {
      ParseNodeType::EXPRESSION,
    } },
  } },
  { ParseNodeType::RETURN_STMT, {
    Production{ {
      TokenType::KW_RETURN,
      ParseNodeType::EXPRESSION,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::CONDITION_CHAIN, {
    Production{ {
      ParseNodeType::IF_CONDITION,
      ParseNodeType::CONDITIONAL_BLOCKS,
    } },
  } },
  { ParseNodeType::IF_CONDITION, {
    Production{ {
      TokenType::KW_IF,
      TokenType::OPEN_PAREN,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_PAREN,
    } },
  } },
  { ParseNodeType::CONDITIONAL_BLOCKS, {
    Production{ {
      ParseNodeType::CONDITIONAL_BLOCKS,
      ParseNodeType::CONDITIONAL_BLOCK_CONNECTOR,
      ParseNodeType::CODE_BLOCK,
    } },
    Production{ {
      ParseNodeType::CODE_BLOCK,
    } },
  } },
  { ParseNodeType::CONDITIONAL_BLOCK_CONNECTOR, {
    Production{ {
      ParseNodeType::ELSE_CONNECTOR,
    } },
    Production{ {
      ParseNodeType::ELSEIF_CONNECTOR,
    } },
  } },
  { ParseNodeType::ELSE_CONNECTOR, {
    Production{ {
      TokenType::KW_ELSE,
    } },
  } },
  { ParseNodeType::ELSEIF_CONNECTOR, {
    Production{ {
      TokenType::KW_ELSE,
      ParseNodeType::IF_CONDITION,
    } },
  } },
  { ParseNodeType::LOOP, {
    Production{ {
      ParseNodeType::WHILE_LOOP,
    } },
    Production{ {
      ParseNodeType::DO_WHILE_LOOP,
    } },
  } },
  { ParseNodeType::WHILE_EXPR, {
    Production{ {
      TokenType::KW_WHILE,
      TokenType::OPEN_PAREN,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_PAREN,
    } },
  } },
  { ParseNodeType::WHILE_BODY, {
    Production{ {
      ParseNodeType::CODE_BLOCK,
    } },
    Production{ {
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::WHILE_LOOP, {
    Production{ {
      ParseNodeType::WHILE_EXPR,
      ParseNodeType::WHILE_BODY,
    } },
  } },
  { ParseNodeType::DO_WHILE_LOOP, {
    Production{ {
      TokenType::KW_DO,
      ParseNodeType::WHILE_BODY,
      ParseNodeType::WHILE_EXPR,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::BREAK_STMT, {
    Production{ {
      TokenType::KW_BREAK,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::CONTINUE_STMT, {
    Production{ {
      TokenType::KW_CONTINUE,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::STRUCT_DECL, {
    Production{ {
      TokenType::KW_STRUCT,
      ParseNodeType::ID,
      TokenType::OPEN_BRACE,
      ParseNodeType::STRUCT_BODY,
      TokenType::CLOSE_BRACE,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::UNION_DECL, {
    Production{ {
      TokenType::BASE_TYPE,
      TokenType::KW_UNION,
      ParseNodeType::ID,
      TokenType::OPEN_BRACE,
      ParseNodeType::UNION_BODY,
      TokenType::CLOSE_BRACE,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::STRUCT_BODY, {
    Production{ {
      ParseNodeType::STRUCT_BODY,
      ParseNodeType::STRUCT_ITEM,
    } },
    Production{ {
      ParseNodeType::STRUCT_ITEM,
    } },
  } },
  { ParseNodeType::UNION_BODY, {
    Production{ {
      ParseNodeType::UNION_BODY,
      ParseNodeType::UNION_ITEM,
    } },
    Production{ {
      ParseNodeType::UNION_ITEM,
    } },
  } },
  { ParseNodeType::STRUCT_ITEM, {
    Production{ {
      ParseNodeType::DECLARATION,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::UNION_ITEM, {
    Production{ {
      ParseNodeType::DECLARATION,
      TokenType::SEMICOLON,
    } },
  } },
  { ParseNodeType::EXPRESSION, {
    Production{ {
      ParseNodeType::ASSIGNMENT_EXPR,
    } },
  } },
  { ParseNodeType::ASSIGNMENT_EXPR, {
    Production{ {
      ParseNodeType::LOGICAL_EXPR,
      ParseNodeType::ASSIGNMENT_OP,
      ParseNodeType::ASSIGNMENT_EXPR,
    } },
    Production{ {
      ParseNodeType::LOGICAL_EXPR,
    } },
  } },
  { ParseNodeType::ASSIGNMENT_OP, {
    Production{ {
      TokenType::EQUALS,
    } },
    Production{ {
      TokenType::PLUS_EQ,
    } },
    Production{ {
      TokenType::MINUS_EQ,
    } },
    Production{ {
      TokenType::STAR_EQ,
    } },
    Production{ {
      TokenType::DIV_EQ,
    } },
    Production{ {
      TokenType::MOD_EQ,
    } },
  } },
  { ParseNodeType::LOGICAL_EXPR, {
    Production{ {
      ParseNodeType::LOGICAL_EXPR,
      ParseNodeType::LOGICAL_OP,
      ParseNodeType::RELATIONAL_EQ_EXPR,
    } },
    Production{ {
      ParseNodeType::RELATIONAL_EQ_EXPR,
    } },
  } },
  { ParseNodeType::LOGICAL_OP, {
    Production{ {
      TokenType::DOUBLE_AMP,
    } },
    Production{ {
      TokenType::DOUBLE_PIPE,
    } },
  } },
  { ParseNodeType::RELATIONAL_EQ_EXPR, {
    Production{ {
      ParseNodeType::RELATIONAL_EQ_EXPR,
      ParseNodeType::RELATIONAL_EQ_OP,
      ParseNodeType::RELATIONAL_COMP_EXPR,
    } },
    Production{ {
      ParseNodeType::RELATIONAL_COMP_EXPR,
    } },
  } },
  { ParseNodeType::RELATIONAL_EQ_OP, {
    Production{ {
      TokenType::DOUBLE_EQ,
    } },
    Production{ {
      TokenType::NOT_EQ,
    } },
    Production{ {
      TokenType::TRIPLE_EQ,
    } },
    Production{ {
      TokenType::TRIPLE_NOT_EQ,
    } },
  } },
  { ParseNodeType::RELATIONAL_COMP_EXPR, {
    Production{ {
      ParseNodeType::RELATIONAL_COMP_EXPR,
      ParseNodeType::RELATIONAL_COMP_OP,
      ParseNodeType::ADDITIVE_EXPR,
    } },
    Production{ {
      ParseNodeType::ADDITIVE_EXPR,
    } },
  } },
  { ParseNodeType::RELATIONAL_COMP_OP, {
    Production{ {
      TokenType::LSS,
    } },
    Production{ {
      TokenType::LEQ,
    } },
    Production{ {
      TokenType::GTR,
    } },
    Production{ {
      TokenType::GEQ,
    } },
  } },
  { ParseNodeType::ADDITIVE_EXPR, {
    Production{ {
      ParseNodeType::ADDITIVE_EXPR,
      ParseNodeType::ADDITIVE_OP,
      ParseNodeType::MULTIPLICATIVE_EXPR,
    } },
    Production{ {
      ParseNodeType::MULTIPLICATIVE_EXPR,
    } },
  } },
  { ParseNodeType::ADDITIVE_OP, {
    Production{ {
      TokenType::PLUS,
    } },
    Production{ {
      TokenType::MINUS,
    } },
  } },
  { ParseNodeType::MULTIPLICATIVE_EXPR, {
    Production{ {
      ParseNodeType::MULTIPLICATIVE_EXPR,
      ParseNodeType::MULTIPLICATIVE_OP,
      ParseNodeType::UNARY_EXPR,
    } },
    Production{ {
      ParseNodeType::UNARY_EXPR,
    } },
  } },
  { ParseNodeType::MULTIPLICATIVE_OP, {
    Production{ {
      TokenType::STAR,
    } },
    Production{ {
      TokenType::DIV,
    } },
    Production{ {
      TokenType::MOD,
    } },
  } },
  { ParseNodeType::UNARY_EXPR, {
    Production{ {
      ParseNodeType::UNARY_OP,
      ParseNodeType::UNARY_EXPR,
    } },
    Production{ {
      ParseNodeType::VARUSE_EXPR,
    } },
  } },
  { ParseNodeType::UNARY_OP, {
    Production{ {
      TokenType::PLUS,
    } },
    Production{ {
      TokenType::MINUS,
    } },
    Production{ {
      TokenType::BANG,
    } },
  } },
  { ParseNodeType::VARUSE_EXPR, {
    Production{ {
      ParseNodeType::MEMBER_ACCESS_EXPR,
    } },
    Production{ {
      ParseNodeType::FUNCTION_CALL_EXPR,
    } },
    Production{ {
      ParseNodeType::SUBSCRIPT_EXPR,
    } },
    Production{ {
      ParseNodeType::VAR_EXPR,
    } },
  } },
  { ParseNodeType::MEMBER_ACCESS_EXPR, {
    Production{ {
      ParseNodeType::VARUSE_EXPR,
      TokenType::PERIOD,
      ParseNodeType::VAR_EXPR,
    } },
  } },
  { ParseNodeType::FUNCTION_CALL_EXPR, {
    Production{ {
      ParseNodeType::VARUSE_EXPR,
      TokenType::OPEN_PAREN,
      ParseNodeType::ARG_LIST,
      TokenType::CLOSE_PAREN,
    } },
  } },
  { ParseNodeType::ARG_LIST, {
    Production{ {
      ParseNodeType::ARG_LIST,
      TokenType::COMMA,
      ParseNodeType::ARG_LIST,
    } },
    Production{ {
      TokenType::COMMA,
      ParseNodeType::ARG_LIST,
    } },
    Production{ {
      ParseNodeType::EXPRESSION,
    } },
  } },
  { ParseNodeType::SUBSCRIPT_EXPR, {
    Production{ {
      ParseNodeType::VARUSE_EXPR,
      TokenType::OPEN_BRACKET,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_BRACKET,
    } },
  } },
  { ParseNodeType::VAR_EXPR, {
    Production{ {
      ParseNodeType::PAREN_EXPR,
    } },
    Production{ {
      ParseNodeType::LIB_ACCESS,
    } },
    Production{ {
      ParseNodeType::ID,
    } },
    Production{ {
      ParseNodeType::LITERAL,
    } },
  } },
  { ParseNodeType::PAREN_EXPR, {
    Production{ {
      TokenType::OPEN_PAREN,
      ParseNodeType::EXPRESSION,
      TokenType::CLOSE_PAREN,
    } },
  } },
  { ParseNodeType::LIB_ACCESS, {
    Production{ {
      ParseNodeType::IMPORT_LIB_NAME,
      ParseNodeType::LIB_ACCESSOR,
      ParseNodeType::ID,
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
      TokenType::DECIMAL_LITERAL,
    } },
    Production{ {
      TokenType::FLOAT_LITERAL,
    } },
    // Production{ {
    //   TokenType::STRING,
    // } },
    Production{ {
      ParseNodeType::BOOLEAN_LITERAL,
    } },
  } },
  { ParseNodeType::BOOLEAN_LITERAL, {
    Production{ {
      TokenType::KW_TRUE,
    } },
    Production{ {
      TokenType::KW_FALSE,
    } },
  } },
};

std::string typeToString(const ParseNodeType& type) {
  switch(type) {
    case ParseNodeType::SOURCE: return "SOURCE";
    case ParseNodeType::SOURCE_BODY: return "SOURCE_BODY";
    case ParseNodeType::IMPORT_GROUP: return "IMPORT_GROUP";
    case ParseNodeType::IMPORT_STATEMENT: return "IMPORT_STATEMENT";
    case ParseNodeType::IMPORT_CONTENTS: return "IMPORT_CONTENTS";
    case ParseNodeType::IMPORT_LIB_NAME: return "IMPORT_LIB_NAME";
    case ParseNodeType::IMPORT_ITEM_GROUP: return "IMPORT_ITEM_GROUP";
    case ParseNodeType::IMPORT_ITEMS: return "IMPORT_ITEMS";
    case ParseNodeType::IMPORT_ITEM: return "IMPORT_ITEM";
    case ParseNodeType::STATEMENTS: return "STATEMENTS";
    case ParseNodeType::STATEMENT: return "STATEMENT";
    case ParseNodeType::STATEMENT_BODY: return "STATEMENT_BODY";
    case ParseNodeType::CODE_BLOCK: return "CODE_BLOCK";
    case ParseNodeType::DECLARATION: return "DECLARATION";
    case ParseNodeType::DECL_SPECIFIER: return "DECL_SPECIFIER";
    case ParseNodeType::DECL_LIST: return "DECL_LIST";
    case ParseNodeType::DECL_ITEM: return "DECL_ITEM";
    case ParseNodeType::INITIALIZER: return "INITIALIZER";
    case ParseNodeType::DECLARATOR: return "DECLARATOR";
    case ParseNodeType::PAREN_DECL: return "PAREN_DECL";
    case ParseNodeType::PTR_DECL: return "PTR_DECL";
    case ParseNodeType::NO_PTR_DECL: return "NO_PTR_DECL";
    case ParseNodeType::ARRAY_DECL: return "ARRAY_DECL";
    case ParseNodeType::FUNC_DECL: return "FUNC_DECL";
    case ParseNodeType::PARAM_LIST: return "PARAM_LIST";
    case ParseNodeType::PARAM_ITEM: return "PARAM_ITEM";
    case ParseNodeType::FUNC_TYPE_DECL: return "FUNC_TYPE_DECL";
    case ParseNodeType::FUNCTION_IMPL: return "FUNCTION_IMPL";
    case ParseNodeType::FUNCTION_ARGUMENTS: return "FUNCTION_ARGUMENTS";
    case ParseNodeType::FUNCTION_ARGUMENT: return "FUNCTION_ARGUMENT";
    case ParseNodeType::DEFAULT_FUNC_ARG_VALUE: return "DEFAULT_FUNC_ARG_VALUE";
    case ParseNodeType::RETURN_STMT: return "RETURN_STMT";
    case ParseNodeType::CONDITION_CHAIN: return "CONDITION_CHAIN";
    case ParseNodeType::IF_CONDITION: return "IF_CONDITION";
    case ParseNodeType::CONDITIONAL_BLOCKS: return "CONDITIONAL_BLOCKS";
    case ParseNodeType::CONDITIONAL_BLOCK_CONNECTOR: return "CONDITIONAL_BLOCK_CONNECTOR";
    case ParseNodeType::ELSE_CONNECTOR: return "ELSE_CONNECTOR";
    case ParseNodeType::ELSEIF_CONNECTOR: return "ELSEIF_CONNECTOR";
    case ParseNodeType::LOOP: return "LOOP";
    case ParseNodeType::WHILE_EXPR: return "WHILE_EXPR";
    case ParseNodeType::WHILE_BODY: return "WHILE_BODY";
    case ParseNodeType::WHILE_LOOP: return "WHILE_LOOP";
    case ParseNodeType::DO_WHILE_LOOP: return "DO_WHILE_LOOP";
    case ParseNodeType::BREAK_STMT: return "BREAK_STMT";
    case ParseNodeType::CONTINUE_STMT: return "CONTINUE_STMT";
    case ParseNodeType::STRUCT_DECL: return "STRUCT_DECL";
    case ParseNodeType::UNION_DECL: return "UNION_DECL";
    case ParseNodeType::STRUCT_BODY: return "STRUCT_BODY";
    case ParseNodeType::UNION_BODY: return "UNION_BODY";
    case ParseNodeType::STRUCT_ITEM: return "STRUCT_ITEM";
    case ParseNodeType::UNION_ITEM: return "UNION_ITEM";
    case ParseNodeType::EXPRESSION: return "EXPRESSION";
    case ParseNodeType::ASSIGNMENT_EXPR: return "ASSIGNMENT_EXPR";
    case ParseNodeType::ASSIGNMENT_OP: return "ASSIGNMENT_OP";
    case ParseNodeType::LOGICAL_EXPR: return "LOGICAL_EXPR";
    case ParseNodeType::LOGICAL_OP: return "LOGICAL_OP";
    case ParseNodeType::RELATIONAL_EQ_EXPR: return "RELATIONAL_EQ_EXPR";
    case ParseNodeType::RELATIONAL_EQ_OP: return "RELATIONAL_EQ_OP";
    case ParseNodeType::RELATIONAL_COMP_EXPR: return "RELATIONAL_COMP_EXPR";
    case ParseNodeType::RELATIONAL_COMP_OP: return "RELATIONAL_COMP_OP";
    case ParseNodeType::ADDITIVE_EXPR: return "ADDITIVE_EXPR";
    case ParseNodeType::ADDITIVE_OP: return "ADDITIVE_OP";
    case ParseNodeType::MULTIPLICATIVE_EXPR: return "MULTIPLICATIVE_EXPR";
    case ParseNodeType::MULTIPLICATIVE_OP: return "MULTIPLICATIVE_OP";
    case ParseNodeType::UNARY_EXPR: return "UNARY_EXPR";
    case ParseNodeType::UNARY_OP: return "UNARY_OP";
    case ParseNodeType::VARUSE_EXPR: return "VARUSE_EXPR";
    case ParseNodeType::MEMBER_ACCESS_EXPR: return "MEMBER_ACCESS_EXPR";
    case ParseNodeType::FUNCTION_CALL_EXPR: return "FUNCTION_CALL_EXPR";
    case ParseNodeType::ARG_LIST: return "ARG_LIST";
    case ParseNodeType::SUBSCRIPT_EXPR: return "SUBSCRIPT_EXPR";
    case ParseNodeType::VAR_EXPR: return "VAR_EXPR";
    case ParseNodeType::PAREN_EXPR: return "PAREN_EXPR";
    case ParseNodeType::LIB_ACCESS: return "LIB_ACCESS";
    case ParseNodeType::LIB_ACCESSOR: return "LIB_ACCESSOR";
    case ParseNodeType::ID: return "ID";
    case ParseNodeType::LITERAL: return "LITERAL";
    case ParseNodeType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
    default: LOG(ERROR) << "FATAL ERROR: UNKNOWN PARSE NODE TYPE: " << static_cast<int>(type) ; throw new std::runtime_error("no stringification for parse node type!");
  }
}
std::ostream& operator<< (std::ostream& out, const ParseNodeType& type) {
  out << typeToString(type);
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