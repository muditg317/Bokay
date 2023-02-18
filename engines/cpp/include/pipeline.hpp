#pragma once

#include "utilities.hpp"

#include <tuple>
#include <type_traits>

#include <boost/filesystem/path.hpp>

template<class ... Stages>
struct next_stage_helper {
  using StagesTuple = std::tuple<Stages...>;
  constexpr static auto Count = sizeof...(Stages);
  template<class Stage>
  constexpr static auto StageIndex = TupleIndexOf_v<Stage, StagesTuple>;
  template<size_t Ind>
  using StageAt = std::tuple_element_t<Ind, StagesTuple>;

  using _Inds = std::make_index_sequence<Count-1>;

  // template<class Stage>
  // struct next_stage {
  //   using type = StageAt<StageIndex<Stage>+1>;
  // };

  // template<size_t ... Inds>
  // static constexpr auto next_stage_map_f(std::index_sequence<Inds...>)
  //     -> decltype(
  //         TypeToType::map<
  //           TypeToType::pair<StageAt<Inds>, StageAt<Inds+1>>...
  //         >) {};
  
  // using next_stage_map = decltype( next_stage_map_f(_Inds{}) );

  // template <class Stage>
  // using next_t = typename next_stage_map::template value<Stage>;

  template<size_t ... Inds>
  struct next_stage_map_s : std::index_sequence<Inds...> {
    using map = TypeToType::map<
                  TypeToType::pair<StageAt<Inds>, StageAt<Inds+1>>...
                >;
  };

  using next_stage_map_ = next_stage_map_s<_Inds{}>;

  template <class Stage>
  using next_t = typename next_stage_map_::map::template value<Stage>;


  //      TypeToType::map<
  //   TypeToType::pair<Stages, next_stage<Stages>>...
  // >;

  // template<class Stage>
  // using next_t = typename next_stage<Stage>::type;

  // template<class Stage, size_t ... Inds>
  // static constexpr auto get_next_stage_helper(std::index_sequence<Inds...>) {
  //   return std::array<std::tuple_element_t<Inds+1, StagesTuple>, Count-1>{};
  // }

  // template<class Stage>
  // static constexpr auto get_next_stage() {
  //   return get_next_stage_helper<Stage>(_Inds{});
  // }

  // template<class Stage, size_t ... Inds>
  // static constexpr auto get_next_stage_index_helper(std::index_sequence<Inds...>) {
  //   return std::array<size_t, Count-1>{TupleIndexOf_v<std::tuple_element_t<Inds+1, StagesTuple>, StagesTuple>...};
  // }

  // template<class Stage>
  // static constexpr auto get_next_stage_index() {
  //   return get_next_stage_index_helper<Stage>(_Inds{});
  // }
};

template<class ... Stages>
class Pipeline {
 public:
  struct PipelineOptions {
    bool outputTemps;
    boost::filesystem::path tmpFileDir;
    std::string sourceName;
  };
 private:
  using StagesTuple = std::tuple<Stages...>;
  constexpr static auto StageCount = std::tuple_size_v<StagesTuple>;
  using FirstStage = std::tuple_element_t<0, StagesTuple>;
  using LastStage = std::tuple_element_t<StageCount-1, StagesTuple>;
  template<class Stage>
  constexpr static auto StageIndex = TupleIndexOf_v<Stage, StagesTuple>;

  using next_stage = next_stage_helper<Stages...>;

  template<class Stage>
  using next_stage_for = next_stage::template next_t<Stage>;

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