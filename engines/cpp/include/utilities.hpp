#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <type_traits>
#include <utility>

#include <boost/filesystem/path.hpp>

// namespace detail {
//         template <std::size_t ... indices>
//         decltype(auto) build_string(const char * str, std::index_sequence<indices...>) {
//                 return std::make_tuple(str[indices]...);
//         }
// }

// template <std::size_t N>
// constexpr decltype(auto) make_string(const char(&str)[N]) {
//         return detail::build_string(str, std::make_index_sequence<N>());
// }


template <char ... Chars>
struct String {
  static constexpr char chars[] = {Chars...};
  static constexpr std::size_t size = sizeof...(Chars);
  // static constexpr char const *const c_str() { return chars; }
};

template <typename Str, std::size_t ... indices>
decltype(auto) build_string(std::index_sequence<indices...>) {
        return String<Str().chars[indices]...>();
}

#define make_string(str) []{\
        struct Str { const char * chars = str; };\
        return build_string<Str>(std::make_index_sequence<sizeof(str)>());\
}()


// #define make_string(str) struct __Str_##str { const char * chars = #str; };
// #define load_string(str) build_string<__Str_##str>(std::make_index_sequence<sizeof(#str)>());


// // redefine macro without lambda
// #undef make_string
// template<int N>
// struct Str {
//   static constexpr char const chars[N] = {};
//   Str(const char *str) {
//     for (int i = 0; i < N; i++) {
//       chars[i] = str[i];
//     }
//   }
// };
// #define make_string(str) build_string<Str<sizeof(str)>>(str, std::make_index_sequence<sizeof(str)>())

// // constexpr string with const member functions
// class str_const { 
// private:
//     const char* const p_;
//     const std::size_t sz_;
// public:

//     template<std::size_t N>
//     constexpr str_const(const char(&a)[N]) : // ctor
//     p_(a), sz_(N-1) {}

//     constexpr char operator[](std::size_t n) const { 
//         return n < sz_ ? p_[n] :
//         throw std::out_of_range("");
//     }

//     constexpr std::size_t size() const { return sz_; } // size()
// };


// template <char... letters>
// struct string_t{
//     static char const * c_str() {
//         static constexpr char string[]={letters...,'\0'};
//         return string;
//     }
// };

// template<str_const const& str,std::size_t... I>
// auto constexpr expand(std::index_sequence<I...>){
//     return string_t<str[I]...>{};
// }

// template<str_const const& str>
// using string_const_to_type = decltype(expand<str>(std::make_index_sequence<str.size()>{}));


template<class Input_t, class Output_t, typename ErrorCode, typename Name, typename TmpOutExt>
class CompilerStage {
  static_assert(std::is_enum<ErrorCode>::value, "ErrorCode must be an enum");
 public:
  constexpr static ErrorCode SUCCESS_CODE = static_cast<ErrorCode>(0);
  static const char *name(void) { return Name::chars; };
  static const char *tmpOutExt(void) { return TmpOutExt::chars; };
  // constexpr static char NAME[] = Name::c_str();

  using InputType = Input_t;
  using OutputType = Output_t;
  using ErrorType = ErrorCode;

  CompilerStage(void) {};
  virtual ErrorCode operator()(Input_t &input, Output_t *&output) const = 0;
  virtual bool writeOutput(Output_t &output, boost::filesystem::path path) const = 0;
};

#endif