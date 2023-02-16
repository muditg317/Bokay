#ifndef PARSER_HPP
#define PARSER_HPP

#include "grammar.hpp"
#include "lexer.hpp"
#include "parser.hpp"

#include <algorithm>
#include <functional>
#include <ostream>
#include <queue>
#include <vector>
#include <variant>

#include <boost/filesystem/path.hpp>


enum class ASTBuilderResult {
  AST_BUILD_SUCCESS,
  FAILED_REDUCTION,
};

class ASTBuilder;

struct ASTNode {
 private:
  // generic information
  std::vector<std::reference_wrapper<ASTNode>> children; // TODO: delete
 public:
  ASTNode() {};

  virtual std::string getName(void) const = 0;
  
  std::string toTabbedString(void) const;
  /**
   * @brief print a degenerate section of the tree in a single line
   * 
   * @param stream where to print the tree nodes to
   * @param prefix the prefix for the tree node names
   * @return ASTNode the lowest descendant of the tree that is not degenerate or a leaf
   */
  const ASTNode &recursivelyPrintDegenerateSubtrees(std::ostream &stream, std::string prefix) const;

  inline bool hasOneChild(void) const {return children.size() == 1;};

  virtual inline bool isLeafElement(void) const = 0;

  inline bool isLeafNode(void) const {
    return hasOneChild() && children[0].get().isLeafElement();
  };

  inline bool isDegenerateTree(void) const {
    return hasOneChild() && !children[0].get().isLeafElement();
  };

  inline bool childTreeIsLeaf(void) const {
    return isDegenerateTree() && children[0].get().isLeafNode();
  };

  friend class ASTBuilder;
};
struct ASTNonLeafNode : public ASTNode {
  ASTNonLeafNode(): ASTNode() {};
  inline bool isLeafElement(void) const override {return false;};
};
struct ASTLeafElement : public ASTNode {
  ASTLeafElement(Token token): ASTNode(), token(token) {};
  inline bool isLeafElement(void) const override {return true;};
  inline std::string getName(void) const override {return token.toShortString();};
 private:
  Token token;
};
std::ostream& operator<<(std::ostream& out, const ASTLeafElement& leafNode);

struct ASTRootNode : public ASTNonLeafNode {
  ASTRootNode() {};
  inline std::string getName(void) const override {return "ASTRootNode";};
};

class ASTBuilder {
  using input_t = struct ParsingTree;
  using output_t = ASTRootNode;
 public:
  ASTBuilder(void);
  ASTBuilderResult run(input_t &parseTree, output_t *&resultTree) const;
  bool writeTree(ASTRootNode &astRoot, boost::filesystem::path filePath) const;
 private:
  bool reduce(ASTNode &astNode, ASTNode &result) const;
};

#endif