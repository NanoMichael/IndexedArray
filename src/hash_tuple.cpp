#include <functional>
#include <iostream>
#include <tuple>

template <size_t I = 0, typename P, typename... Ts>
constexpr auto tuple_fold_left(const P& op, const std::tuple<Ts...>& t) {
  if constexpr(I == sizeof...(Ts) - 1) {
    return std::get<I>(t);
  } else {
    return op(std::get<I>(t), tuple_fold_left<I + 1>(op, t));
  }
}

template <size_t I, typename P, typename... Ts>
constexpr auto __tuple_fold_right(const P& op, const std::tuple<Ts...>& t) {
  if constexpr(I == 0) {
    return std::get<I>(t);
  } else {
    return op(std::get<I>(t), __tuple_fold_right<I - 1>(op, t));
  }
}

template <typename P, typename... Ts>
constexpr auto tuple_fold_right(const P& op, const std::tuple<Ts...>& t) {
  return __tuple_fold_right<sizeof...(Ts) - 1>(op, t);
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

int main(int argc, char* argv[]) {
  const auto a = std::make_tuple(1, 2, 3);
  std::cout << "sum of tuple (left): " << tuple_fold_left(std::plus<>{}, a) << std::endl;
  std::cout << "sum of tuple (right): " << tuple_fold_right(std::plus<>{}, a) << std::endl;
  std::cout << "hash of tuple (left): " << tuple_fold_left(combine_hash<void>{}, a) << std::endl;
  std::cout << "hash of tuple (right): " << tuple_fold_right(combine_hash<void>{}, a) << std::endl;

  std::cout
    << "hash of tuple (hash): "
    << tuple_hash<std::decay<decltype(a)>::type>{}(a)
    << std::endl;
  return 0;
}
