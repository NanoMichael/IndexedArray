#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <iostream>
#include <ratio>
#include <vector>
#include <unordered_map>
#include <algorithm>

template <class T, size_t N, size_t M>
class IndexedArray {
private:
  const T* const _raw;
  const size_t _rows;

  int get(int& l, int& h, const int offset, const T& t) const {
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const int v = *(_raw + (m * N) + offset);;
      if (v == t) return m;
      t < v ? h = m - 1 : l = m + 1;
    }
    return -1;
  }

  int last(int l, int h, const int offset, const T& t) const {
    int r = -1;
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const int v = *(_raw + (m * N) + offset);

      if (t < v) {
        h = m - 1;
      } else if (t > v) {
        l = m + 1;
      } else {
        r = m;
        l = m + 1;
      }
    }
    return r;
  }

  int first(int l, int h, const int offset, const T& t) const {
    int r = -1;
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const int v = *(_raw + (m * N) + offset);

      if (t < v) {
        h = m - 1;
      } else if (t > v) {
        l = m + 1;
      } else {
        r = m;
        h = m - 1;
      }
    }
    return r;
  }

public:
  IndexedArray(const T* arr, int len) : _raw(arr), _rows(len / N) {}

  const T* operator()(const T args[M]) const {
    int l = 0, h = _rows;
    for (int i = 0; i < M; i++) {
      const int r = get(l, h, i, args[i]);
      if (r < 0) return NULL;
      if (i == M - 1) return _raw + (r * N);
      int v, t;
      for (int j = i + 1; j < M; j++) {
        v = *(_raw + (r * N) + j);
        t = args[j];
        if (v != t) break;
      }
      if (t < v) {
        h = r - 1;
        l = first(l, h, i, args[i]);
      } else if (t > v) {
        l = r + 1;
        h = last(l, h, i, args[i]);
      } else {
        return _raw + (r * N);
      }
    }
    return NULL;
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
    printf("[%2d, %2d, %2d]", arr[j * 3 + 0], arr[j * 3 + 1], arr[j * 3 + 2]);
    if (j == i) printf(" <- %d", i);
    printf("\n");
  }
}

void generate_data(int*& arr, std::unordered_map<Arg, int, Hash, Eq>& map) {
  std::vector<Arg> data;
  data.reserve(SIZE);
  // radom generate, allow duplicate
  srand((int) time(NULL));
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

void benchmark_ia(int* arr) {
  const IndexedArray<int, 3, 3> ia(arr, SIZE * 3);

  printf("find in arr:\n");
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  int args[3] = {};
  for (int i = 0; i < SIZE; i++) {
    const int start = i * 3;
    args[0] = arr[start + 0];
    args[1] = arr[start + 1];
    args[2] = arr[start + 2];
    const int* r = ia(args);
    if (r == NULL
      || r[0] != args[0]
      || r[1] != args[1]
      || r[2] != args[2]) {
      show_context(arr, i);
    }
    assert(r != NULL);
    assert(r[0] == args[0] && r[1] == args[1] && r[2] == args[2]);
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "took " << time_span.count() << " milliseconds.\n"  << std::endl;
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
  std::cout << "took " << time_span.count() << " milliseconds.\n"  << std::endl;
}

void benchmark() {
  int* arr = new int[SIZE * 3];
  std::unordered_map<Arg, int, Hash, Eq> map;
  generate_data(arr, map);

  benchmark_ia(arr);
  benchmark_map(map, arr);
}

int main(int argc, char* argv[]) {
  benchmark();
  return 0;
}
