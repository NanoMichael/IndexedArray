#include <cstdlib>
#include <tuple>

template <class T, size_t N, size_t M>
class BinaryArray {
private:
  const T* const _raw;
  const size_t _rows;

public:
  BinaryArray() = delete;

  BinaryArray(const BinaryArray& arr) = delete;

  explicit BinaryArray(const T* arr, int len) : _raw(arr), _rows(len / N) {}
  
  template<T... K>
  const T* operator()(const K&... keys) const {
    int l = 0, h = _rows;
    while (l <= h) {
      const int m = l + ((h - l) >> 1);
      const T* r = _raw + (m * N);
      auto tuple_key = std::make_tuple(keys);
    }
  }
};
