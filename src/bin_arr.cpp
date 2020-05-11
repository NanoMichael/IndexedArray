#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ratio>
#include <unordered_map>
#include <vector>

template <typename T, size_t N, size_t M>
class BinaryArray {
private:
  const T* const _raw;
  const size_t _rows;

  bool lt(const T* const a, const T* const b) const {
    for (size_t i = 0; i < M; i++) {
      if (a[i] < b[i]) return true;
      if (a[i] > b[i]) return false;
    }
    return false;
  }

  bool eq(const T* const a, const T* const b) const {
    for (size_t i = 0; i < M; i++) {
      if (a[i] != b[i]) return false;
    }
    return true;
  }

public:
  BinaryArray() = delete;

  BinaryArray(const BinaryArray& arr) = delete;

  explicit BinaryArray(const T* arr, int len) : _raw(arr), _rows(len / N) {}

  template <typename... Ks>
  const T* operator()(const Ks&... keys) const {
    const T k[] = {keys...};
    int l = 0, h = _rows;
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const T* r = _raw + (m * N);
      if (eq(k, r)) return r;
      lt(k, r) ? h = m - 1 : l = m + 1;
    }
    return nullptr;
  }
};

struct Arg {
  int a, b, c;

  Arg(int x, int y, int z) : a(x), b(y), c(z) {}

  bool operator==(const Arg& x) const {
    return a == x.a && b == x.b && c == x.c;
  }
};

typedef struct {
  bool operator()(const Arg& a1, const Arg& a2) const {
    return a1 == a2;
  }
} Eq;

typedef struct {
  size_t operator()(const Arg& a) const {
    size_t result = 1;
    result = 31 * result + a.a;
    result = 31 * result + a.b;
    result = 31 * result + a.c;
    return result;
  }
} Hash;

using namespace std::chrono;

#define SIZE 10000000
#define RANDOM_MAX 999
#define CONTEXT_LIMIT 20
#define random(a, b) (rand() % (b - a + 1) + a)

void show_context(const int* arr, const int i, const int limit = CONTEXT_LIMIT) {
  const int start = std::max(0, i - limit);
  const int end = std::min(SIZE, i + limit);
  for (int j = start; j < end; j++) {
    printf("%2d, %2d, %2d,", arr[j * 3 + 0], arr[j * 3 + 1], arr[j * 3 + 2]);
    if (j == i) printf(" <- %d", i);
    printf("\n");
  }
}

void generate_data(int* arr, std::unordered_map<Arg, int, Hash, Eq>& map) {
  std::vector<Arg> data;
  data.reserve(SIZE);
  // radom generate, allow duplicate
  srand((int)time(NULL));
  while (data.size() < SIZE) {
    const Arg arg(random(0, RANDOM_MAX), random(0, RANDOM_MAX), random(0, RANDOM_MAX));
    data.push_back(arg);
    map[arg] = 0;
  }
  // sort the vector
  std::sort(std::begin(data), std::end(data), [](const Arg& x, const Arg& y) {
    auto a = std::make_tuple(x.a, x.b, x.c);
    auto b = std::make_tuple(y.a, y.b, y.c);
    return a < b;
  });
  // convert vector to array
  for (int i = 0; i < SIZE; i++) {
    const int start = i * 3;
    const Arg& x = data[i];
    arr[start + 0] = x.a;
    arr[start + 1] = x.b;
    arr[start + 2] = x.c;
  }
}

bool eq(const int* r, const int* a, int start) {
  return r[0] == a[start + 0] && r[1] == a[start + 1] && r[2] == a[start + 2];
}

void benchmark_ba(int* arr) {
  const BinaryArray<int, 3, 3> ba(arr, SIZE * 3);

  printf("find in arr:\n");
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  for (int i = 0; i < SIZE; i++) {
    const int start = i * 3;
    const int* r = ba(arr[start + 0], arr[start + 1], arr[start + 2]);
    if (r == nullptr || !eq(r, arr, start)) {
      show_context(arr, i);
    }
    assert(r != nullptr);
    assert(eq(r, arr, start));
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "took " << time_span.count() << " milliseconds.\n"
            << std::endl;
}

void benchmark_map(const std::unordered_map<Arg, int, Hash, Eq>& map, const int* arr) {
  printf("find in map:\n");
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  Arg args(0, 0, 0);
  for (int i = 0; i < SIZE; i++) {
    const int start = i * 3;
    args.a = arr[start + 0];
    args.b = arr[start + 1];
    args.c = arr[start + 2];
    auto it = map.find(args);
    if (it == map.end()) {
      printf("[%d, %d, %d]\n", args.a, args.b, args.c);
      show_context(arr, i);
    }
    assert(it != map.end());
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "took " << time_span.count() << " milliseconds.\n"
            << std::endl;
}

void benchmark() {
  int* arr = new int[SIZE * 3];
  std::unordered_map<Arg, int, Hash, Eq> map;
  generate_data(arr, map);

  benchmark_ba(arr);
  benchmark_map(map, arr);
}

int main(int argc, char* argv[]) {
  benchmark();
  return 0;
}
