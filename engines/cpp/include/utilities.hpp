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
  // static_assert(always_false<T>, "T was not found in tuple");
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


template <typename MappingTarget>
struct TypeToValue {
  template <class, MappingTarget>
  struct pair{ };

  template <class, class, MappingTarget>
  struct get_tuple
  { using type = std::tuple<>;
    static constexpr std::tuple<> value_tuple = std::make_tuple();
  };

  template <class Stage, MappingTarget MT>
  struct get_tuple<Stage, Stage, MT>
  { using type = std::tuple<MappingTarget>; 
    static constexpr MappingTarget value = MT;
    static constexpr std::tuple<MappingTarget> value_tuple = std::make_tuple(MT);
  };

  template <typename ...>
  struct map;

  template <class ... Stages, MappingTarget ... MTs>
  struct map<pair<Stages, MTs>...>
  {
    template <class Stage>
    static constexpr MappingTarget find_value() {
      return std::get<0>( std::tuple_cat(
        get_tuple<Stage, Stages, MTs>::value_tuple...));
    }

    template <class Stage>
    static constexpr MappingTarget value = find_value<Stage>();
  };
};

struct TypeToType {
  template <class, typename>
  struct pair
  { };

  template <class, class, typename>
  struct get_tuple
  { using type = std::tuple<>; };

  template <class Stage, typename T>
  struct get_tuple<Stage, Stage, T>
  { using type = std::tuple<T>; };

  template <typename ...>
  struct map;

  template <class ... Stages, typename ... Ts>
  struct map<pair<Stages, Ts>...>
  {
    template <class Stage>
    static constexpr auto find_value_func ()
        -> decltype( std::get<0>( std::tuple_cat(
            std::declval<typename get_tuple<Stage, Stages, Ts>::type>()...)) );

    template <class Stage>
    using value = std::remove_reference_t<decltype( find_value_func<Stage>() )>;
  };
};

struct TypeToString {
  template <class, StringLiteral>
  struct pair{ };

  template <class, class, StringLiteral>
  struct get_tuple
  { using type = std::tuple<>;
    static constexpr std::tuple<> value_tuple = std::make_tuple();
  };

  template <class Stage, StringLiteral SL>
  struct get_tuple<Stage, Stage, SL>
  { using type = std::tuple<decltype(SL)>; 
    static constexpr auto value = SL;
    static constexpr auto value_string = SL.value;
    static constexpr type value_tuple = std::make_tuple(SL);
  };

  template <typename ...>
  struct map;

  template <class ... Stages, StringLiteral ... SLs>
  struct map<pair<Stages, SLs>...>
  {
    using length_map = TypeToValue<size_t>::map<
      TypeToValue<size_t>::pair<Stages, sizeof(SLs.value)>...
    >;

    template <class Stage>
    static constexpr auto str_len = length_map::template value<Stage>;

    // static_assert(length_map::template value<Lexer> == 4, "Lexer tok length is not 4");
    template <class Stage>
    static constexpr StringLiteral<str_len<Stage>> find_value() {
      return std::get<0>( std::tuple_cat(
        get_tuple<Stage, Stages, SLs>::value_tuple...));
    }

    template <class Stage>
    static constexpr StringLiteral<str_len<Stage>> str_lit_value = find_value<Stage>();

    template <class Stage>
    static constexpr auto value = str_lit_value<Stage>.value;
    // static constexpr auto value = get_tuple<Stage, find_value<Stage>()>::value;
  };
};

#define CREATE_MAPPING(result_type, mapping_name, mapping_type) \
  using mapping_name##_map = mapping_type::map< \
    ALL_MAP_ENTRIES \
  >; \
  template<class Stage> \
  result_type mapping_name = mapping_name##_map::value<Stage>;
#define MAPPING_ENTRY(mapping_type, Stage, value) \
    mapping_type::pair< \
      Stage, value \
    >

#endif