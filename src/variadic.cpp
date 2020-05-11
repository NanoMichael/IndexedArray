#include <iostream>

template<typename... Ts>
struct tuple;

template<> struct tuple<> {};

using empty_tuple = tuple<>;

template<typename T, typename... Ts>
struct tuple<T, Ts...> : tuple<Ts...> {
  using base_t = tuple<Ts...>;
  using this_t = tuple<T, Ts...>;
  using value_t = T;

  T value;

  tuple<T, Ts...>() : value(T()), base_t() {}

  explicit tuple<T, Ts...>(const T& t, const Ts&... ts) : value(t), base_t(ts...) {}

  bool operator<(const tuple<T, Ts...>& t) const {
    if (value < t.value) return true;
    if (value > t.value) return false;
    return ((base_t) *this) < ((base_t) t);
  }

  bool operator==(const tuple<T, Ts...>& t) const {
    if (value != t.value) return false;
    return ((base_t) *this) == ((base_t) t);
  }
};

template<typename T>
struct tuple<T> : empty_tuple {
  using base_t = empty_tuple;
  using this_t = tuple<T>;
  using value_t = T;

  T value;

  tuple<T>() : value(T()) {}

  explicit tuple<T>(const T& t) : value(t) {}

  bool operator<(const tuple<T>& t) const {
    return value < t.value;
  }

  bool operator==(const tuple<T>& t) const {
    return value == t.value;
  }
};

template<std::size_t I, typename... Ts>
struct tuple_get;

template<std::size_t I, typename T, typename... Ts>
struct tuple_get<I, tuple<T, Ts...>> {
  using value_t = typename tuple_get<I - 1, tuple<Ts...>>::value_t;
  using tuple_t = typename tuple_get<I - 1, tuple<Ts...>>::tuple_t;
};

template<typename T, typename... Ts>
struct tuple_get<0, tuple<T, Ts...>> {
  using value_t = T;
  using tuple_t = tuple<T, Ts...>;
};

template<>
struct tuple_get<0, empty_tuple> {
  using value_t = void;
  using tuple_t = empty_tuple;
};

template<std::size_t I, typename... Ts>
typename tuple_get<I, tuple<Ts...>>::value_t& get(tuple<Ts...>& t) {
  using tuple_t = typename tuple_get<I, tuple<Ts...>>::tuple_t;
  return ((tuple_t*)&t)->value;
}

template<typename... Ts>
tuple<Ts...> make_tuple(const Ts&... ts) {
  return tuple<Ts...>{ts...};
}

int main(int argc, char* argv[]) {
  auto a = make_tuple(3, std::string("pi"), 3.14f);
  auto b = make_tuple(3, std::string("pi"), 3.14f);
  printf("[%d, %s, %f]\n", get<0>(a), get<1>(a).c_str(), get<2>(a));
  printf("[%d, %s, %f]\n", get<0>(b), get<1>(b).c_str(), get<2>(b));
  printf("b < a: %d\n", b < a);
  printf("a == b: %d\n", b == a);
  return 0;
}
