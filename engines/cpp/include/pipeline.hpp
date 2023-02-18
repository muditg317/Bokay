#pragma once

#include "utilities.hpp"
#include "compiler-stage.hpp"

#include <tuple>
#include <type_traits>

#include <boost/filesystem/path.hpp>

template<class ... Stages>
struct next_stage_helper {
 private:
  struct magic_type {};
 public:
  using StagesTuple = std::tuple<Stages...>;
  constexpr static auto Count = sizeof...(Stages);
  template<class Stage>
  constexpr static auto StageIndex = TupleIndexOf_v<Stage, StagesTuple>;
  template<size_t Ind>
  using StageAt = std::tuple_element_t<Ind, StagesTuple>;

  template<size_t ... Inds>
  static constexpr auto next_stage_map_f(std::index_sequence<Inds...>)
      ->  TypeToType::map<
            TypeToType::pair<StageAt<Inds>, StageAt<Inds+1>>...,
            TypeToType::pair<StageAt<Count-1>, magic_type>
          >;
  
  using next_stage_map = decltype( next_stage_map_f(
      std::make_index_sequence<Count-1>{}
    ) );

  template <class Stage>
  using next_t = typename next_stage_map::template value<Stage>;
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
  using StagesTuple = std::tuple<Stages...>;
  constexpr static auto StageCount = std::tuple_size_v<StagesTuple>;
  using FirstStage = std::tuple_element_t<0, StagesTuple>;
  using LastStage = std::tuple_element_t<StageCount-1, StagesTuple>;
  template<class Stage>
  constexpr static auto StageIndex = TupleIndexOf_v<Stage, StagesTuple>;
  template<size_t Ind>
  using StageAt = std::tuple_element_t<Ind, StagesTuple>;

  template<class Stage>
  using next_stage = typename next_stage_helper<Stages...>::template next_t<Stage>;

 public:
  Pipeline() = default;

  typename LastStage::Base::OutputType &operator()(
    typename FirstStage::Base::InputType &input,
    PipelineOptions options
  ) const; // TODO: make this a template with start/end stages

 private:

  StagesTuple stages;

  using OutputPtrTuple = transform_tuple_t<StageOutputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;
  using InputPtrTuple = transform_tuple_t<StageInputTypeTransformer::transform<StagesTuple>, std::add_pointer_t>;

  template<class Stage>
  typename Stage::Base::OutputType &runStage(
    typename Stage::Base::InputType &input,
    typename Stage::Base::OutputType *&output,
    PipelineOptions options
  ) const;
};

#include "pipeline.ipp"