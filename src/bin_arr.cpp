#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <ratio>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "tuple_hash.hpp"

template <typename T, size_t N, size_t M>
class BinaryArray {
private:
  const T* const _raw;
  const size_t   _rows;

  int compare(const T a[M], const T b[M]) const {
    for (size_t i = 0; i < M; i++) {
      if (a[i] < b[i]) return -1;
      if (a[i] > b[i]) return 1;
    }
    return 0;
  }

public:
  BinaryArray() = delete;

  BinaryArray(const BinaryArray& arr) = delete;

  explicit BinaryArray(const T* arr, int len) : _raw(arr), _rows(len / N) {}

  template <typename... Ks>
  const T* operator()(const Ks&... keys) const {
    const T k[] = {keys...};
    int     l = 0, h = _rows;
    while (l <= h) {
      const int m   = l + ((h - l) >> 1);
      const T*  r   = _raw + (m * N);
      const int cmp = compare(k, r);
      if (cmp == 0) return r;
      cmp < 0 ? h = m - 1 : l = m + 1;
    }
    return nullptr;
  }
};

using namespace std::chrono;

#define RANDOM_MAX    999
#define CONTEXT_LIMIT 20
#define random(a, b)  (rand() % (b - a + 1) + a)

void show_context(const int* arr, const size_t size, const int i, const int limit = CONTEXT_LIMIT) {
  const int start = std::max(0, i - limit);
  const int end   = std::min(size, (size_t)(i + limit));
  for (int j = start; j < end; j++) {
    printf("%2d, %2d, %2d,", arr[j * 3 + 0], arr[j * 3 + 1], arr[j * 3 + 2]);
    if (j == i) printf(" <- %d", i);
    printf("\n");
  }
}

using Arg  = std::tuple<int, int, int>;
using Hash = tuple_hash<Arg>;

void generate_data(
    int*                                arr,
    const size_t                        size,
    std::unordered_map<Arg, int, Hash>& hash_map,
    std::map<Arg, int>&                 map) {
  std::vector<Arg> data;
  data.reserve(size);
  // radom generate, allow duplicate
  srand((int)time(NULL));
  while (data.size() < size) {
    const Arg arg{random(0, RANDOM_MAX), random(0, RANDOM_MAX), random(0, RANDOM_MAX)};
    data.push_back(arg);
    hash_map[arg] = 0;
    map[arg]      = 0;
  }
  // sort the vector
  std::sort(std::begin(data), std::end(data), [](const Arg& x, const Arg& y) {
    return x < y;
  });
  // convert vector to array
  for (int i = 0; i < size; i++) {
    const int  start = i * 3;
    const Arg& x     = data[i];
    arr[start + 0]   = std::get<0>(x);
    arr[start + 1]   = std::get<1>(x);
    arr[start + 2]   = std::get<2>(x);
  }
}

bool eq(const int* r, const int* a, int start) {
  return r[0] == a[start + 0] && r[1] == a[start + 1] && r[2] == a[start + 2];
}

template <typename F>
double count_time(const F& f) {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  f();
  high_resolution_clock::time_point t2        = high_resolution_clock::now();
  duration<double, std::milli>      time_span = t2 - t1;
  return time_span.count();
}

void benchmark_ba(int* arr, const size_t size) {
  const BinaryArray<int, 3, 3> ba(arr, size * 3);
  for (int i = 0; i < size; i++) {
    const int  start = i * 3;
    const int* r     = ba(arr[start + 0], arr[start + 1], arr[start + 2]);
    if (r == nullptr || !eq(r, arr, start)) {
      show_context(arr, size, i);
    }
    assert(r != nullptr);
    assert(eq(r, arr, start));
  }
}

template <typename Map>
void benchmark_map(const Map& map, const int* arr, const size_t size) {
  Arg args{0, 0, 0};
  for (int i = 0; i < size; i++) {
    const int start   = i * 3;
    std::get<0>(args) = arr[start + 0];
    std::get<1>(args) = arr[start + 1];
    std::get<2>(args) = arr[start + 2];
    auto it           = map.find(args);
    if (it == map.end()) {
      printf("[%d, %d, %d]\n", std::get<0>(args), std::get<1>(args), std::get<2>(args));
      show_context(arr, size, i);
    }
    assert(it != map.end());
  }
}

auto run_epoch(const size_t size) {
  int*                               arr = new int[size * 3];
  std::unordered_map<Arg, int, Hash> hash_map;
  std::map<Arg, int>                 map;
  generate_data(arr, size, hash_map, map);

  auto arr_time  = count_time([arr, size] { benchmark_ba(arr, size); });
  auto map_time  = count_time([&map, arr, size] { benchmark_map(map, arr, size); });
  auto hash_time = count_time([&hash_map, arr, size] { benchmark_map(hash_map, arr, size); });

  delete[] arr;

  return std::make_tuple(arr_time, map_time, hash_time);
}

void unit_KorM(const int n, std::string& s) {
  std::stringstream ss;
  if (n > 100000)
    ss << (n / 1000000) << "M";
  else
    ss << (n / 1000) << "K";
  ss >> s;
}

void benchmark() {
  size_t data_size = 1000;
  printf("%5s %8s %8s %8s\n", "SIZE", "BA", "MAP", "HASH");
  for (int i = 0; i < 5; i++) {
    auto        durations = run_epoch(data_size);
    std::string s;
    unit_KorM(data_size, s);
    printf(
        "%5s %8.2lf %8.2lf %8.2lf\n",
        s.c_str(),
        std::get<0>(durations),
        std::get<1>(durations),
        std::get<2>(durations));
    data_size *= 10;
  }
}

int main(int argc, char* argv[]) {
  benchmark();
  return 0;
}
