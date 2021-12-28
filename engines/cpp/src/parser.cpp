#include "parser.hpp"

#include <map>

const std::map<ParseNodeType, GrammarNodeRHS> grammarNodeMap {
  { ParseNodeType::IMPORT_STATEMENT, GrammarNodeRHS{} },
};