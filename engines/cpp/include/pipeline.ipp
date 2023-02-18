#pragma once

#include "logging-inc.hpp"

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

  InputPtrTuple inputs;
  std::get<StageIndex<FirstStage>>(inputs) = &input;
  typename LastStage::Base::OutputType *output = nullptr;

  auto applyInputsFromOutputs = [&]<std::size_t... index_seq>(std::index_sequence<index_seq...>) {
    (runStage<std::tuple_element_t<index_seq, StagesTuple>>(
      *std::get<StageIndex<std::tuple_element_t<index_seq, StagesTuple>>>(inputs),
      std::get<StageIndex<next_stage_for<std::tuple_element_t<index_seq, StagesTuple>>>>(inputs),
      options
    ), ...);
  };
  applyInputsFromOutputs(std::make_index_sequence<StageCount-1>{});

  runStage<LastStage>(*std::get<StageIndex<LastStage>>(inputs), output, options);

  return *output;
}