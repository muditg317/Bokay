#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "utilities.hpp"
#include "args-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast-builder.hpp"

#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>

#include <boost/filesystem/path.hpp>
// #include <boost/filesystem.hpp>

enum class CompilerResult {
  COMPILATION_SUCCESS,
  INVALID_COMPILATION_OPTIONS,
  FAILED_LEXING,
  FAILED_PARSING,
  FAILED_AST_BUILDING,
};

template <typename MappingTarget>
struct CompilerStageMapping {
  using mapped_type = MappingTarget;
  template<StringLiteral StageName, MappingTarget MT_val>
  struct csm_pair { 
    constexpr static StringLiteral key = StageName;
    constexpr static MappingTarget value = MT_val;

    template <StringLiteral get_pair_SN>
    static csm_pair get_pair() {
      static_assert(StageName.value == get_pair_SN.value, "Stage name mismatch");
      return {};
    }
  };

  template <typename ... Ps>
  struct csm_map : public Ps...
  {
    using Ps::get_pair...;

    // template <StringLiteral StageName>
    // constexpr static mapped_type find_type(void) {
    //   return get_pair<StageName>();
    // }
  };

  template <StringLiteral StageName, MappingTarget MT_val>
  struct ct_pair{ };

  template <StringLiteral, StringLiteral, MappingTarget>
  struct get_tuple
  { using type = std::tuple<>;
    static constexpr std::tuple<> value_tuple = std::make_tuple();
  };

  template <StringLiteral StageName, MappingTarget MT>
  struct get_tuple<StageName, StageName, MT>
  { using type = std::tuple<MappingTarget>; 
    static constexpr MappingTarget value = MT;
    static constexpr std::tuple<MappingTarget> value_tuple = std::make_tuple(MT);
  };

  template <typename ...>
  struct ct_map;

  template <StringLiteral ... SNs, MappingTarget ... MTs>
  struct ct_map<ct_pair<SNs, MTs>...>
  {
    // template <StringLiteral SN>
    // static constexpr auto find_type_func ()
    // -> decltype( std::get<0>( std::tuple_cat(
    //           std::declval<typename get_tuple<SN, SNs, MTs>::type>()...)) );

    // template <std::size_t I>
    // using find_type
    //   = std::remove_reference_t<decltype( find_type_func<I>() )>;
    // template <StringLiteral SN>
    // using find_type
    //   = std::remove_reference_t<decltype( find_type_func<SN>() )>;

    template <StringLiteral SN>
    static constexpr MappingTarget find_type() {
      return std::get<0>( std::tuple_cat(
        get_tuple<SN, SNs, MTs>::value_tuple...));
    }

    template <StringLiteral SN>
    static constexpr MappingTarget type = find_type<SN>();

  };


};


// float foo = Lexer::NAME;
using CompilerCodeMapping = CompilerStageMapping<CompilerResult>;
using compiler_code_map = CompilerCodeMapping::csm_map<CompilerCodeMapping::csm_pair<Lexer::NAME_, CompilerResult::FAILED_LEXING>,
                                                       CompilerCodeMapping::csm_pair<Parser::NAME_, CompilerResult::FAILED_PARSING>,
                                                       CompilerCodeMapping::csm_pair<ASTBuilder::NAME_, CompilerResult::FAILED_AST_BUILDING>>;
// CompilerResult lexerFailCode = compiler_code_map::find_type<Lexer::NAME_>();

using ccode_ct_map = CompilerCodeMapping::ct_map<CompilerCodeMapping::ct_pair<Lexer::NAME_, CompilerResult::FAILED_LEXING>,
                                                 CompilerCodeMapping::ct_pair<Parser::NAME_, CompilerResult::FAILED_PARSING>,
                                                 CompilerCodeMapping::ct_pair<ASTBuilder::NAME_, CompilerResult::FAILED_AST_BUILDING>>;
// using ResType = ccode_ct_map::find_type<Lexer::NAME_>;
static_assert(ccode_ct_map::find_type<Lexer::NAME_>() == CompilerResult::FAILED_LEXING, "Failed to find type");
static_assert(ccode_ct_map::type<Lexer::NAME_> == CompilerResult::FAILED_LEXING, "Failed to find type");
// ResType foo = 34.0f;

class Compiler {
  public:
    Compiler(Options &options);
    CompilerResult run(void);
  private:
    Lexer lexer;
    Parser parser;
    ASTBuilder astBuilder;

    template<class Stage, CompilerResult FailureCode = ccode_ct_map::type<Stage::NAME_>>
    typename Stage::Base::OutputType &runStage(
      Stage &stage,
      typename Stage::Base::InputType &input
    ) const;

    bool validate_options(void);
    bool outputTemps;
    std::string sourceName;
    boost::filesystem::path sourceFile;
    boost::filesystem::path tempFileDir;
    boost::filesystem::path outputPath;
    std::string fileContents;
};

#endif