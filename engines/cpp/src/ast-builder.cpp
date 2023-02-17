#include "ast-builder.hpp"
#include "parser.hpp"
#include "lexer.hpp"
#include "grammar.hpp"

#include <algorithm>
#include <deque>
#include <functional>
#include <map>
#include <ostream>
#include <typeinfo>
#include <utility>
#include <vector>

#include <boost/filesystem.hpp>
#include <glog/logging.h>

std::ostream& operator<<(std::ostream& out, const ASTLeafElement& leafNode) {
  return out << leafNode.getName();
}

std::string ASTNode::toTabbedString(void) const {
  std::ostringstream oss{"\n"};
  std::deque<std::pair<std::reference_wrapper<const ASTNode>,size_t>> treeNodeQueue{}; // node,depth queue
  treeNodeQueue.push_front(std::make_pair(std::cref(*this), static_cast<size_t>(0)));
  while (!treeNodeQueue.empty()) {
    const ASTNode &node = treeNodeQueue.front().first.get();
    size_t depth = treeNodeQueue.front().second;
    treeNodeQueue.pop_front();
    if (node.isLeafElement()) { // node is a leaf
      oss << std::string(depth, '\t') << dynamic_cast<const ASTLeafElement&>(node) << '\n';
      continue;
    }
    const ASTNode &subtree = node;

    if (subtree.isLeafNode()) {
      oss << std::string(depth, '\t') << subtree.getName() << " - " << dynamic_cast<const ASTLeafElement&>(subtree.children[0].get()) << '\n';
    } else if (subtree.isDegenerateTree()) {
      oss << std::string(depth, '\t') << subtree.getName();
      const ASTNode &descendant_tree =
          subtree.children[0].get()
            .recursivelyPrintDegenerateSubtrees(oss, " > ");
      if (descendant_tree.isLeafNode()) {
        oss << " - " << descendant_tree.getName() << ':' << descendant_tree.children[0].get().getName() << '\n';
      } else {
        oss << " > " << descendant_tree.getName() << '\n';
        for (std::vector<std::reference_wrapper<ASTNode>>::const_reverse_iterator i = descendant_tree.children.rbegin(); 
            i != descendant_tree.children.rend(); ++i ) {
          treeNodeQueue.push_front(std::make_pair(std::cref(*i), depth+1));
        }
      }
    } else {
      oss << std::string(depth, '\t') << subtree.getName() << '\n';
      for (std::vector<std::reference_wrapper<ASTNode>>::const_reverse_iterator i = subtree.children.rbegin(); 
          i != subtree.children.rend(); ++i ) {
        treeNodeQueue.push_front(std::make_pair(std::cref(*i), depth+1));
      }
    }
  }
  return oss.str();
}

const ASTNode &ASTNode::recursivelyPrintDegenerateSubtrees(std::ostream &stream, std::string prefix) const {
  if (children.size() > 1) {
    return *this;
  }
  if (isLeafNode() || childTreeIsLeaf()) {
    return *this;
  }
  stream << prefix << getName();
  return children[0].get().recursivelyPrintDegenerateSubtrees(stream, prefix);
}



ASTBuilder::ASTBuilder(void) : Base() {}

ASTBuilder::Base::ErrorType ASTBuilder::operator()(Base::InputType &parseTree, Base::OutputType *&resultTree) const {
  DLOG(INFO) << "Begin AST reduction on parse tree";

  // if reached: parsing finished successfully!
  DLOG(INFO) << "AST reduction succeeded! Printing tree...";
  ASTRootNode finalTree; // TODO: replace with actual ASTNode
  DLOG(INFO) << finalTree.toTabbedString();
  resultTree = new ASTRootNode(finalTree);

  return ASTBuilderResult::AST_BUILDING_SUCCESS;
}

bool ASTBuilder::reduce(ASTNode &astNode, ASTNode &result) const {
  DLOG(INFO) << "REDUCTION";
  return false;
}

bool ASTBuilder::writeOutput(Base::OutputType &astRoot, boost::filesystem::path filePath) const {
  if (!boost::filesystem::exists(filePath.parent_path())) {
    LOG(ERROR) << "Cannot write ptree to " << filePath << " because parent dir does not exist!" ;
    return false;
  }

  boost::filesystem::ofstream astFile{filePath};

  astFile << astRoot.toTabbedString();

  astFile.close();
  return true;
}