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
  template <class, MappingTarget>
  struct csm_pair{ };

  template <class, class, MappingTarget>
  struct get_tuple
  { using type = std::tuple<>;
    static constexpr std::tuple<> value_tuple = std::make_tuple();
  };

  template <class Stage, MappingTarget MT>
  struct get_tuple<Stage, Stage, MT>
  { using type = std::tuple<MappingTarget>; 
    static constexpr MappingTarget value = MT;
    static constexpr std::tuple<MappingTarget> value_tuple = std::make_tuple(MT);
  };

  template <typename ...>
  struct csm_map;

  template <class ... Stages, MappingTarget ... MTs>
  struct csm_map<csm_pair<Stages, MTs>...>
  {
    template <class Stage>
    static constexpr MappingTarget find_type() {
      return std::get<0>( std::tuple_cat(
        get_tuple<Stage, Stages, MTs>::value_tuple...));
    }

    template <class Stage>
    static constexpr MappingTarget type = find_type<Stage>();
  };
};

struct CompilerStageTypeMapping {
  template <class, typename>
  struct cstm_pair
  { };

  template <class, class, typename>
  struct get_tuple
  { using type = std::tuple<>; };

  template <class Stage, typename T>
  struct get_tuple<Stage, Stage, T>
  { using type = std::tuple<T>; };

  template <typename ...>
  struct cstm_map;

  template <class ... Stages, typename ... Ts>
  struct cstm_map<cstm_pair<Stages, Ts>...>
  {
    template <class Stage>
    static constexpr auto find_type_func ()
    -> decltype( std::get<0>( std::tuple_cat(
              std::declval<typename get_tuple<Stage, Stages, Ts>::type>()...)) );

    template <class Stage>
    using type
      = std::remove_reference_t<decltype( find_type_func<Stage>() )>;
  };
};



using CompilerCodeMapping = CompilerStageMapping<CompilerResult>;
using compiler_failure_code_map = CompilerCodeMapping::csm_map<CompilerCodeMapping::csm_pair<Lexer, CompilerResult::FAILED_LEXING>,
                                                               CompilerCodeMapping::csm_pair<Parser, CompilerResult::FAILED_PARSING>,
                                                               CompilerCodeMapping::csm_pair<ASTBuilder, CompilerResult::FAILED_AST_BUILDING>>;
template<class Stage>
CompilerResult failure_code_for = compiler_failure_code_map::type<Stage>;

using CompilerNextStageMapping = CompilerStageTypeMapping::cstm_map<CompilerStageTypeMapping::cstm_pair<Lexer, Parser>,
                                                                    CompilerStageTypeMapping::cstm_pair<Parser, ASTBuilder>,
                                                                    CompilerStageTypeMapping::cstm_pair<ASTBuilder, void>>;
template<class Stage>
using next_stage_for = CompilerNextStageMapping::type<Stage>;

class Compiler {
 public:
  Compiler(Options &options);
  CompilerResult run(void);
 private:
  using StagesTuple = std::tuple<Lexer, Parser, ASTBuilder>;
  StagesTuple stages;
  using OutputPtrTuple = transform_tuple_t<StageOutputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;
  using InputPtrTuple = transform_tuple_t<StageInputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;
  // Lexer lexer;
  // Parser parser;
  // ASTBuilder astBuilder;

  template<class Stage>
  typename Stage::Base::OutputType &runStage(
    typename Stage::Base::InputType &input
  ) const;

  template<class ... Stages>
  typename std::tuple_element_t<sizeof...(Stages)-1, std::tuple<Stages...>>::Base::OutputType &runStages(
    typename std::tuple_element_t<0, std::tuple<Stages...>>::Base::InputType &input
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