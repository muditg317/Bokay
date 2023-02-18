#ifndef COMPILER_HPP
#define COMPILER_HPP

#include "utilities.hpp"
#include "compiler-stage.hpp"
#include "args-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ast-builder.hpp"

#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

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
  struct pair{ };

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
  struct map;

  template <class ... Stages, MappingTarget ... MTs>
  struct map<pair<Stages, MTs>...>
  {
    template <class Stage>
    static constexpr MappingTarget find_value() {
      return std::get<0>( std::tuple_cat(
        get_tuple<Stage, Stages, MTs>::value_tuple...));
    }

    template <class Stage>
    static constexpr MappingTarget value = find_value<Stage>();
  };
};

struct CompilerStageTypeMapping {
  template <class, typename>
  struct pair
  { };

  template <class, class, typename>
  struct get_tuple
  { using type = std::tuple<>; };

  template <class Stage, typename T>
  struct get_tuple<Stage, Stage, T>
  { using type = std::tuple<T>; };

  template <typename ...>
  struct map;

  template <class ... Stages, typename ... Ts>
  struct map<pair<Stages, Ts>...>
  {
    template <class Stage>
    static constexpr auto find_value_func ()
        -> decltype( std::get<0>( std::tuple_cat(
            std::declval<typename get_tuple<Stage, Stages, Ts>::type>()...)) );

    template <class Stage>
    using value = std::remove_reference_t<decltype( find_value_func<Stage>() )>;
  };
};

struct CompilerStageStringMapping {
  template <class, StringLiteral>
  struct pair{ };

  template <class, class, StringLiteral>
  struct get_tuple
  { using type = std::tuple<>;
    static constexpr std::tuple<> value_tuple = std::make_tuple();
  };

  template <class Stage, StringLiteral SL>
  struct get_tuple<Stage, Stage, SL>
  { using type = std::tuple<decltype(SL)>; 
    static constexpr auto value = SL;
    static constexpr auto value_string = SL.value;
    static constexpr type value_tuple = std::make_tuple(SL);
  };

  template <typename ...>
  struct map;

  template <class ... Stages, StringLiteral ... SLs>
  struct map<pair<Stages, SLs>...>
  {
    using length_map = CompilerStageMapping<size_t>::map<
      CompilerStageMapping<size_t>::pair<Stages, sizeof(SLs.value)>...
    >;

    template <class Stage>
    static constexpr auto str_len = length_map::template value<Stage>;

    // static_assert(length_map::template value<Lexer> == 4, "Lexer tok length is not 4");
    template <class Stage>
    static constexpr StringLiteral<str_len<Stage>> find_value() {
      return std::get<0>( std::tuple_cat(
        get_tuple<Stage, Stages, SLs>::value_tuple...));
    }

    template <class Stage>
    static constexpr StringLiteral<str_len<Stage>> str_lit_value = find_value<Stage>();

    template <class Stage>
    static constexpr auto value = str_lit_value<Stage>.value;
    // static constexpr auto value = get_tuple<Stage, find_value<Stage>()>::value;
  };
};

#define LEXER X(Lexer, tok, FAILED_LEXING, Parser)
#define PARSER X(Parser, ptree, FAILED_PARSING, ASTBuilder)
#define AST_BUILDER X(ASTBuilder, ast, FAILED_AST_BUILDING, void)

#define X_delim , // must be reset to , after use (if changed)

#define ALL_STAGES \
LEXER X_delim \
PARSER X_delim \
AST_BUILDER

#define CREATE_MAPPING(result_type, mapping_name, mapping_type) \
  using mapping_name##_map = mapping_type::map< \
    ALL_STAGES \
  >; \
  template<class Stage> \
  result_type mapping_name = mapping_name##_map::value<Stage>;
#define MAPPING_ENTRY(mapping_type, Stage, value) \
    mapping_type::pair< \
      Stage, value \
    >

#define X(Stage, ext, compRes, next) MAPPING_ENTRY(CompilerStageMapping<CompilerResult>, Stage, CompilerResult::compRes)
CREATE_MAPPING(constexpr auto, failure_code_for, CompilerStageMapping<CompilerResult>)
#undef X

#define X(Stage, ext, compRes, next) MAPPING_ENTRY(CompilerStageStringMapping, Stage, #ext)
CREATE_MAPPING(constexpr auto, tmp_file_ext, CompilerStageStringMapping)
#undef X

#define X(Stage, ext, compRes, next) MAPPING_ENTRY(CompilerStageTypeMapping, Stage, next)
CREATE_MAPPING(using, next_stage_for, CompilerStageTypeMapping)
#undef X

class Compiler {
 public:
  Compiler(Options &options);
  CompilerResult operator()(void);
 private:
  #define X(Stage, ext, compRes, next) Stage
  using StagesTuple = std::tuple<ALL_STAGES>;
  #undef X
  constexpr static auto StageCount = std::tuple_size_v<StagesTuple>;
  using FirstStage = std::tuple_element_t<0, StagesTuple>;
  using LastStage = std::tuple_element_t<StageCount-1, StagesTuple>;
  template<class Stage>
  constexpr static auto StageIndex = TupleIndexOf_v<Stage, StagesTuple>;

  StagesTuple stages;

  using OutputPtrTuple = transform_tuple_t<StageOutputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;
  using InputPtrTuple = transform_tuple_t<StageInputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;
  using InputPtrRefTuple = transform_tuple_t<transform_tuple_t<InputPtrTuple, std::reference_wrapper>, std::type_identity_t>;

  template<class Stage>
  typename Stage::Base::OutputType &runStage(
    typename Stage::Base::InputType &input,
    typename Stage::Base::OutputType *&output
  ) const;

  LastStage::Base::OutputType &runStages(
    FirstStage::Base::InputType &input
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