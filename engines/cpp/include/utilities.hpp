#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <algorithm>
#include <type_traits>

#include <boost/filesystem/path.hpp>

template<size_t N>
struct StringLiteral {
  constexpr StringLiteral(const char (&str)[N]) {
      std::copy_n(str, N, value);
  }
  char value[N];
};

template<class Input_t, class Output_t, typename ErrorCode, StringLiteral Name, StringLiteral TmpOutExt>
class CompilerStage {
  static_assert(std::is_enum<ErrorCode>::value, "ErrorCode must be an enum");
 public:
  constexpr static ErrorCode SUCCESS_CODE = static_cast<ErrorCode>(0);
  // static const char *name(void) { return String_Name::chars; };
  constexpr static auto NAME = Name.value;
  constexpr static auto TMP_OUT_EXT = TmpOutExt.value;
  // static const char *name(void) { return Name.value; };
  // static const char *tmpOutExt(void) { return TmpOutExt::chars; };
  // constexpr static char NAME[] = Name::c_str();

  using InputType = Input_t;
  using OutputType = Output_t;
  using ErrorType = ErrorCode;

  CompilerStage(void) {};
  virtual ErrorCode operator()(Input_t &input, Output_t *&output) const = 0;
  virtual bool writeOutput(Output_t &output, boost::filesystem::path path) const = 0;
  virtual void debugCallback(Output_t &output) const = 0;
};

#endif