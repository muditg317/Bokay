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

template <class T, class Tuple>
constexpr auto TupleIndexOf_v = TupleIndexOf<T, Tuple>::value;

template <typename Tuple, template <typename...> typename Component>
struct transform_tuple;

template <typename... Ts, template <typename...> typename Component>
struct transform_tuple<std::tuple<Ts...>, Component> {
    using type = std::tuple<Component<Ts>...>;
};

template <typename Tuple, template <typename...> typename Component>
using transform_tuple_t = typename transform_tuple<Tuple, Component>::type;

template <template <typename...> typename Component>
struct tuple_transformer {
  template <typename Tuple>
  using transform = transform_tuple_t<Tuple, Component>;
};

constexpr bool strings_equal(char const * a, char const * b) {
    return *a == *b && (*a == '\0' || strings_equal(a + 1, b + 1));
}

#endif