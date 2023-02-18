#pragma once

#include <utilities.hpp>

template <class Stage>
using StageInputType = typename Stage::InputType;
template <class Stage>
using StageOutputType = typename Stage::OutputType;

using StageInputTypeTransformer = tuple_transformer<StageInputType>;
using StageOutputTypeTransformer = tuple_transformer<StageOutputType>;

template<class Input_t, class Output_t, typename ErrorCode, StringLiteral Name, StringLiteral TmpOutExt>
class CompilerStage {
  static_assert(std::is_enum<ErrorCode>::value, "ErrorCode must be an enum");
 public:
  constexpr static ErrorCode SUCCESS_CODE = static_cast<ErrorCode>(0);
  constexpr static auto NAME = Name.value;
  constexpr static StringLiteral NAME_ = Name;
  constexpr static auto TMP_OUT_EXT = TmpOutExt.value;

  using InputType = Input_t;
  using OutputType = Output_t;
  using ErrorType = ErrorCode;

  // using NameType = Name;
  // using TmpOutExtType = TmpOutExt;

  CompilerStage(void) {};
  virtual ErrorCode operator()(Input_t &input, Output_t *&output) const = 0;
  virtual bool writeOutput(Output_t &output, boost::filesystem::path path) const = 0;
  virtual void debugCallback(Output_t &output) const = 0;
};