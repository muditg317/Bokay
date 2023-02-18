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
  constexpr StringLiteral(const StringLiteral<N> &other) {
      std::copy_n(other.value, N, value);
  }
  char value[N];
};

template<typename... T>
constexpr bool always_false = false;

template <class T, class Tuple>
struct TupleIndexOf;

template <class T>
struct TupleIndexOf<T, std::tuple<>> {
  static_assert(always_false<T>, "T was not found in tuple");
  static const std::size_t value = 0;
};

template <class T, class... Types>
struct TupleIndexOf<T, std::tuple<T, Types...>> {
    static const std::size_t value = 0;
};

template <class T, class U, class... Types>
struct TupleIndexOf<T, std::tuple<U, Types...>> {
    static const std::size_t value = 1 + TupleIndexOf<T, std::tuple<Types...>>::value;
};

template <typename Tuple, template <typename...> typename Component>
struct transform_tuple;

template <typename... Ts, template <typename...> typename Component>
struct transform_tuple<std::tuple<Ts...>, Component> {
    using type = std::tuple<Component<Ts>...>;
};

template <template <typename...> typename Component>
struct tuple_transformer {
  template <typename Tuple>
  using transform = typename transform_tuple<Tuple, Component>::type;
};

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

#endif