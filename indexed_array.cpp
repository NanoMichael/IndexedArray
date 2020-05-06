#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ratio>
#include <unordered_map>

template <class T, size_t N, size_t M>
class IndexedArray {
private:
  const T* const _raw;
  const size_t _rows;

  int get(int& l, int& h, const int offset, const T& t) {
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const int v = *(_raw + (m * N) + offset);
      if (v == t) return m;
      t < v ? h = m - 1 : l = m + 1;
    }
    return -1;
  }

  int last(int l, int h, const int offset, const T& t) {
    int r = -1;
    while (l <= h) {
      int m = l + ((h - l + 1) >> 1);
      int v = *(_raw + (m * N) + offset);

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

  int first(int l, int h, const int offset, const T& t) {
    int r = -1;
    while (l <= h) {
      int m = l + ((h - l + 1) >> 1);
      int v = *(_raw + (m * N) + offset);

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

  const T* operator()(const T args[M]) {
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
    return (a.a + a.b + a.c) % 512;
  }
} Hash;

using namespace std::chrono;

const int v[] = {
    1, 3, 2, 1,
    2, 4, 3, 2,
    4, 5, 4, 1,
    5, 1, 1, 2,
    6, 4, 2, 1,
    6, 4, 3, 2,
    6, 4, 4, 3,
    6, 4, 6, 4,
    6, 4, 7, 5,
    8, 1, 2, 1,
    8, 5, 5, 2,
    9, 1, 0, 3
};
IndexedArray<int, 4, 3> arr(v, 48);

void test_arr() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  printf("find by indexed array:\n");
  for (int j = 0; j < 10000000; j++) {
    for (int i = 0; i < 12; i++) {
      const int* p = v + (i * 4);
      const int args[] = {p[0], p[1], p[2]};
      const int* r = arr(args);
      // printf("[%d, %d, %d, %d]\n", r[0], r[1], r[2], r[3]);
    }
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "It took me " << time_span.count() << " milliseconds.\n";
  std::cout << std::endl;
}

#define random(a, b) (rand() % (b - a + 1) + a)

void test_arr_not_found() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  printf("find by arr not found:\n");
  srand((int)time(NULL));
  for (int i = 0; i < 10000000; i++) {
    const int args[] = {random(0, 10), random(0, 10), random(0, 10)};
    const int* r = arr(args);
    // printf("find for [%d, %d, %d] -> ", args[0], args[1], args[2]);
    // if (r == NULL) {
    //   printf("NULL\n");
    // } else {
    //   printf("[%d, %d, %d, %d]\n", r[0], r[1], r[2], r[3]);
    // }
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "It took me " << time_span.count() << " milliseconds.\n";
  std::cout << std::endl;
}

std::unordered_map<Arg, int, Hash, Eq> map = {
  {{1, 3, 2}, 1},
  {{2, 4, 3}, 2},
  {{4, 5, 4}, 1},
  {{5, 1, 1}, 2},
  {{6, 4, 2}, 1},
  {{6, 4, 3}, 2},
  {{6, 4, 4}, 3},
  {{6, 4, 6}, 4},
  {{6, 4, 7}, 5},
  {{8, 1, 2}, 1},
  {{8, 5, 5}, 2},
  {{9, 1, 0}, 3}
};

void test_map() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  printf("find by map:\n");
  for (int j = 0; j < 10000000; j++) {
    for (int i = 0; i < 12; i++) {
      const int* p = v + (i * 4);
      const Arg args(p[0], p[1], p[2]);
      auto it = map.find(args);
      // printf("[%d, %d, %d, %d]\n", it->first.a, it->first.b, it->first.c, it->second);
    }
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "It took me " << time_span.count() << " milliseconds.\n";
  std::cout << std::endl;
}

void test_map_not_found() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  printf("find by map not found:\n");
  srand((int)time(NULL));
  for (int i = 0; i < 10000000; i++) {
    const Arg arg(random(0, 100), random(0, 10), random(0, 10));
    auto it = map.find(arg);
    // printf("find for [%d, %d, %d] -> ", arg.a, arg.b, arg.c);
    // if (it == map.end()) {
    //   printf("NULL\n");
    // } else {
    //   printf("[%d, %d, %d, %d]\n", it->first.a, it->first.b, it->first.c, it->second);
    // }
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  duration<double, std::milli> time_span = t2 - t1;
  std::cout << "It took me " << time_span.count() << " milliseconds.\n";
  std::cout << std::endl;
}

int main(int argc, char* argv[]) {
  test_arr();
  test_arr_not_found();
  test_map();
  test_map_not_found();
  return 0;
}
