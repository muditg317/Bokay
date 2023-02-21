#pragma once

#include "utilities.hpp"
#include "compiler-stage.hpp"

#include <tuple>
#include <type_traits>

#include <boost/filesystem/path.hpp>

#define STAGE_INFO_USING_DECLS(Stages) \
using StagesTuple = std::tuple<Stages...>; \
constexpr static auto StageCount = sizeof...(Stages); \
using FirstStage = std::tuple_element_t<0, StagesTuple>; \
using LastStage = std::tuple_element_t<StageCount-1, StagesTuple>; \
template<class Stage> \
constexpr static auto StageIndex = TupleIndexOf_v<Stage, StagesTuple>; \
template<size_t Ind> \
using StageAt = std::tuple_element_t<Ind, StagesTuple>;


template<class ... Stages>
struct next_stage_helper {
 private:
  struct magic_type {};
 public:
  STAGE_INFO_USING_DECLS(Stages)

  template<size_t ... Inds>
  static constexpr auto next_stage_map_f(std::index_sequence<Inds...>)
      ->  TypeToType::map<
            TypeToType::pair<StageAt<Inds>, StageAt<Inds+1>>...,
            TypeToType::pair<StageAt<StageCount-1>, magic_type>
          >;
  
  using next_stage_map = decltype( next_stage_map_f(
      std::make_index_sequence<StageCount-1>{}
    ) );

  template <class Stage>
  using next_t = typename next_stage_map::template value<Stage>;
};

template<class ... Stages>
struct stages_tuple_helper {
  STAGE_INFO_USING_DECLS(Stages)

  template<class Stage>
  using next_stage = typename next_stage_helper<Stages...>::template next_t<Stage>;

  // template <class ...>
  // struct stages_tuple_s;

  // template<class Start, class ... Rest>
  // struct stages_tuple_s<Start, Start> {
  //   using type = std::tuple<Start>;
  // };

  // template<class Start, class End>
  // struct stages_tuple_s<next_stage<Start>, End> {
  //   using type = std::tuple<Start, End>;
  // };

  template<class Start, class End>
  struct stages_tuple_s {
    using type = std::conditional_t<
      std::is_same_v<Start, End>,
      std::tuple<Start>,
      tuple_post_add<
        typename stages_tuple_helper<Stages...>::template stages_tuple_s<next_stage<Start>, End>::type,
        Start
      >
    >;
  };

  template<class Start, class End>
  using stages_tuple_t = typename stages_tuple_s<Start, End>::type;
};

template<class ... Stages>
class Pipeline {
 public:
  struct PipelineOptions {
    bool outputTemps;
    boost::filesystem::path tempFileDir;
    std::string sourceName;
  };
 private:
  STAGE_INFO_USING_DECLS(Stages)

  template<class Stage>
  using next_stage = typename next_stage_helper<Stages...>::template next_t<Stage>;
  static_assert(std::is_same_v<
      next_stage<FirstStage>,
      std::tuple_element_t<1, StagesTuple>
    >, "NextStage is not the same as SecondStage");

  template<class Start, class End>
  using stages_tuple = typename stages_tuple_helper<Stages...>::template stages_tuple_t<Start, End>;
  static_assert(std::is_same_v<
      stages_tuple<next_stage<FirstStage>, LastStage>,
      std::tuple<next_stage<FirstStage>, LastStage>
    >, "PipelineDataTuple is not the same as StagesTuple");

 public:
  Pipeline() = default;

  template<class StartStage = FirstStage, class EndStage = LastStage>
  typename EndStage::Base::OutputType &operator()(
    typename StartStage::Base::InputType &input,
    PipelineOptions options
  ) const; // TODO: make this a template with start/end stages

  // disallow calling with one stage only
  template<class Stage>
  typename Stage::Base::OutputType &operator()(
    typename Stage::Base::InputType &input,
    PipelineOptions options
  ) const = delete;

 private:

  StagesTuple stages;

  using OutputPtrTuple = transform_tuple_t<StageOutputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;
  using InputPtrTuple = transform_tuple_t<StageInputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;

  template<class StartStage, class EndStage>
  struct PipelineDataTuple_helper {
    using type = tuple_pre_add<
      typename StartStage::Base::InputType,
      transform_tuple_t<StageOutputTypeTransformer::transform<
        stages_tuple<
          next_stage<StartStage>,
          EndStage
        >
      >, std::add_pointer_t>
    >;
  };
  
  template<class StartStage = FirstStage, class EndStage = LastStage>
  using PipelineDataTuple = typename PipelineDataTuple_helper<StartStage, EndStage>::type;

  template<class Stage>
  typename Stage::Base::OutputType &runStage(
    typename Stage::Base::InputType &input,
    typename Stage::Base::OutputType *&output,
    PipelineOptions options
  ) const;
};

#include "pipeline.ipp"