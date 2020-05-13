#include <functional>
#include <tuple>

template <size_t I = 0, typename P, typename... Ts>
constexpr auto tuple_fold_left(const P& p, const std::tuple<Ts...>& t) {
  if constexpr (I == sizeof...(Ts) - 1) {
    return std::get<I>(t);
  } else {
    return p(std::get<I>(t), tuple_fold_left<I + 1>(p, t));
  }
}

template <size_t I, typename P, typename... Ts>
constexpr auto __tuple_fold_right(const P& p, const std::tuple<Ts...>& t) {
  if constexpr (I == 0) {
    return std::get<I>(t);
  } else {
    return p(std::get<I>(t), __tuple_fold_right<I - 1>(p, t));
  }
}

template <typename P, typename... Ts>
constexpr auto tuple_fold_right(const P& p, const std::tuple<Ts...>& t) {
  return __tuple_fold_right<sizeof...(Ts) - 1>(p, t);
}

template <typename T>
struct combine_hash;

template <>
struct combine_hash<void> {
  template <typename T, typename U>
  constexpr size_t operator()(const T& t, const U& u) const {
    return std::hash<T>{}(t) + 31 * u;
  }
};

template <typename... Ts>
struct tuple_hash;

template <typename... Ts>
struct tuple_hash<std::tuple<Ts...>> {
  size_t operator()(const std::tuple<Ts...>& t) const {
    return tuple_fold_left(combine_hash<void>{}, t);
  }
};
