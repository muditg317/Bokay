#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include "lexer.hpp"

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

enum class ParseNodeType {
  SOURCE, // [imports] [statements]
  SOURCE_BODY, // [import_group] [statements]
  IMPORT_GROUP, // [import_group] [import_stmnt] | [import_stmnt]
  IMPORT_STATEMENT, // import [lib_name] , { method, otherField }  from [string] ;
  LIB_NAME, // [id]

  STATEMENTS, // [statements] [statement] | [statement]
  STATEMENT, // [stmt_body] ; | [function_impl] | [return_stmt] | [condition_chain] // TODO: | [while_loop]
  STATEMENT_BODY, // [declaration] | [expression]
  CODE_BLOCK, // { [statements] }

  // declarations - C/C++ style
  DECLARATION, // [decl_specifier] [decl_list]
  DECL_SPECIFIER, // [base_type] // TODO: | [struct_decl] | [union_decl]
  DECL_LIST, // [decl_list],[decl_item] | [decl_item]
  DECL_ITEM, // [declarator] | [declarator] equals [initializer]
  INITIALIZER, // [expression]

  // declarators - everything between the base type and initializers for declarations
  DECLARATOR, // [id] | [paren_decl] | [ptr_decl] | [array_decl] | [func_decl]
  PAREN_DECL, // ( [declarator] )
  PTR_DECL, //. *[declarator] // TODO pointers
  NO_PTR_DECL, // [id] | [paren_decl] | [array_decl] | [func_decl]
  ARRAY_DECL, // [no_ptr_decl] [ [expression] ]
  FUNC_DECL, // [no_ptr_decl] ( [param_list] )
  // param lists
  PARAM_LIST, // [param_list],[param_item] | [param_item]
  PARAM_ITEM, // [func_type_decl]
  
  // FUNCTIONS
  FUNC_TYPE_DECL, // [decl_specifier] [no_ptr_decl]
  FUNCTION_IMPL, // [func_type_decl] ( [func_args] ) [code_block]
  FUNCTION_ARGUMENTS, // [func_args],[func_arg] | [func_arg]
  FUNCTION_ARGUMENT, // [func_type_decl] = [default_func_arg_value] | [func_type_decl]
  DEFAULT_FUNC_ARG_VALUE, // [expression] // TODO: type checking for const stuff
  RETURN_STMT, // return [expression] ;

  // TODO: CONDITIONALS, LOOPS,
  CONDITION_CHAIN, // [if_condition] [conditional_blocks]
  IF_CONDITION, // if ( [expression] )
  CONDITIONAL_BLOCKS, // [conditional_blocks] [conditional_block_connector] [code_block] | [code_block]
  CONDITIONAL_BLOCK_CONNECTOR, // [else_connector] | [elseif_connector]
  ELSE_CONNECTOR, // else
  ELSEIF_CONNECTOR, // else [if_condition]

  // TODO: STRUCT_DEF, UNION_DEF,

  EXPRESSION, // [assignment_expr]
  ASSIGNMENT_EXPR, // [logical_expr] [assignment_op] [assignment_expr] | [logical_expr]
  ASSIGNMENT_OP, // = | += | -= | *= | /= | %= // TODO: | <<= | >>= | &= | |= | ^=
  LOGICAL_EXPR, // [logical_expr] [logical_op] [relational_eq_expr] | [relational_eq_expr]
  LOGICAL_OP, // && | ||
  RELATIONAL_EQ_EXPR, // [relational_eq_expr] [relational_eq_op] [relational_comp_expr] | [relational_comp_expr]
  RELATIONAL_EQ_OP, // == | != | === | !==
  RELATIONAL_COMP_EXPR, // [relational_comp_expr] [relational_comp_op] [additive_expr] | [additive_expr]
  RELATIONAL_COMP_OP, // < | > | <= | >=
  ADDITIVE_EXPR, // [additive_expr] [additive_op] [multiplicative_expr] | [multiplicative_expr]
  ADDITIVE_OP, // + | -
  MULTIPLICATIVE_EXPR, // [multiplicative_expr] [multiplicative_op] [unary_expr] | [unary_expr]
  MULTIPLICATIVE_OP, //. * | / | %
  UNARY_EXPR, // [unary_op] [unary_expr] | [varuse_expr]
  UNARY_OP, // + | - | ! // TODO: | ~ | ++ | -- // TODO pointers: | & | *
  VARUSE_EXPR, // [member_access_expr] | [function_call_expr] | [subscript_expr] | [var_expr]
  MEMBER_ACCESS_EXPR, // [varuse_expr] period [var_expr]
  FUNCTION_CALL_EXPR, // [varuse_expr] ( [arg_list] )
  ARG_LIST, // [arg_list] , [arg_list] | , [arg_list] | [expression]
  SUBSCRIPT_EXPR, // [varuse_expr] [ [expression] ]
  
  VAR_EXPR, // [paren_expr] | [lib_access] | [id] | [literal]
  PAREN_EXPR, // ( [expression] )
  LIB_ACCESS, // [lib_name] [lib_accessor] [id]
  LIB_ACCESSOR, // ::
  ID, // variable // [word]

  LITERAL, // decimal | float | string | boolean
  BOOLEAN_LITERAL, // true | false
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

extern const std::map<const ParseNodeType, const std::vector<Production>> grammarRuleMap;

inline std::vector<Production> getRules(ParseNodeType type) {
  return grammarRuleMap.at(type);
}
inline Production getRule(ParseNodeType type, int i = 0) {
  return grammarRuleMap.at(type)[i];
}


#endif