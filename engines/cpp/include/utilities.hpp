#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <type_traits>

#include <boost/filesystem/path.hpp>

template<class Input_t, class Output_t, typename ErrorCode>
class CompilerStage {
  static_assert(std::is_enum<ErrorCode>::value, "ErrorCode must be an enum");
 public:
  using InputType = Input_t;
  using OutputType = Output_t;
  using ErrorType = ErrorCode;

  CompilerStage(void) {};
  virtual ErrorCode operator()(Input_t &input, Output_t *&output) = 0;
  virtual bool writeOutput(Output_t &output, boost::filesystem::path path) = 0;
};

#endif