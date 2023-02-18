#pragma once

#include "logging-inc.hpp"

#include <tuple>
#include <utility>

#include <boost/filesystem/path.hpp>
#include <fmt/format.h>

template<class... Stages>
template<class Stage>
typename Stage::Base::OutputType &Pipeline<Stages...>::runStage(
    typename Stage::Base::InputType &input,
    typename Stage::Base::OutputType *&output,
    PipelineOptions options
  ) const {

  const Stage &stage = std::get<Stage>(stages);

  typename Stage::Base::ErrorType errorCode = stage(input, output);

  if (errorCode != Stage::Base::SUCCESS_CODE) {
    LOG(ERROR) << "Stage " << Stage::Base::NAME << " failed with error code " << static_cast<int>(errorCode) ;
    // throw compiler_code_map::type<Stage::NAME_>;
    // throw failure_code_for<Stage>;
    throw 0;
  }

  #ifdef DEBUG
  stage.debugCallback(*output);
  #endif

  if (options.outputTemps) {
    boost::filesystem::path tempFile = options.tempFileDir / fmt::format("{}.{}", options.sourceName, Stage::Base::TMP_OUT_EXT);
    LOG(INFO) << "Writing " << Stage::Base::NAME << " outputs to temp file: " << tempFile;
    stage.writeOutput(*output, tempFile);
  }

  return *output;
}

template<class... Stages>
typename Pipeline<Stages...>::LastStage::Base::OutputType &Pipeline<Stages...>::operator()(
  typename FirstStage::Base::InputType &input,
  PipelineOptions options
) const {

  using LastOutputPtrTuple = std::tuple<std::tuple_element_t<StageIndex<LastStage>, OutputPtrTuple>>;
  using PipelineDataTuple = decltype(std::tuple_cat(std::declval<InputPtrTuple>(), std::declval<LastOutputPtrTuple>()));
  PipelineDataTuple pipelineData;
  std::get<StageIndex<FirstStage>>(pipelineData) = &input;

  auto applyInputsFromOutputs = [&]<std::size_t... index_seq>(std::index_sequence<index_seq...>) {
    (runStage<StageAt<index_seq>>(
      *std::get<StageIndex<StageAt<index_seq>>>(pipelineData),
      std::get<StageIndex<next_stage<StageAt<index_seq>>>>(pipelineData),
      options
    ), ...);
  };
  applyInputsFromOutputs(std::make_index_sequence<StageCount>{});

  return *std::get<StageIndex<next_stage<LastStage>>>(pipelineData);
}