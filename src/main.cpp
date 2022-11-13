#include <iostream>
#include <typeinfo>

#include "json.hpp"

template <typename T>
using value_type_t = typename T::value_type;

int main() {
  char str[] = "Hello World!";
  otter::json j1 = str;
  std::cout << "Init finished" << std::endl;
  j1.debug_print_str();

  /* TODO(creepy-otter): Add compatible template functions using
   * std::enable_if to support arbitary assginment */
  return 0;
}