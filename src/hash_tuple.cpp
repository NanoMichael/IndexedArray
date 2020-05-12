#include <iostream>

#include "tuple_hash.hpp"

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
