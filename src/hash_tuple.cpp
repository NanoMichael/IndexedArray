#include <functional>
#include <iostream>
#include <tuple>

template <typename... Ts>
struct tuple_hash;

template <typename T, typename... Ts>
struct tuple_hash<std::tuple<T, Ts...>> {
  size_t operator()(const std::tuple<T, Ts...>& t) {
    return std::hash<T>(std::get<0, std::tuple<T, Ts...>>(t)) +
           31 * tuple_hash<std::tuple<Ts...>>{}((std::tuple<Ts...>)t);
  }
};

template <>
struct tuple_hash<std::tuple<>> {
  size_t operator()(const std::tuple<>& t) {
    return 0;
  }
};

int main(int argc, char* argv[]) {
  const auto a = std::make_tuple(0, 1, 2);
  const size_t h = tuple_hash<int, int, int>{}(a);
  std::cout << h << std::endl;
  return 0;
}
