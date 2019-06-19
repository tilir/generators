#include <iostream>

#include "tuples_mixed.hpp"

int main () {
  int numbers[] = {6, 5, 4};

  std::cout << "first pass" << std::endl;
  MixedModeTuple mt(std::begin(numbers), std::end(numbers));
  do {
    for (auto it = mt.begin(); it != mt.end(); ++it)
      std::cout << *it << " ";
    std::cout << std::endl;
  } while(mt.next_tuple());

  std::cout << "once again" << std::endl;
  do {
    for (auto it = mt.begin(); it != mt.end(); ++it)
      std::cout << *it << " ";
    std::cout << std::endl;
  } while(mt.next_tuple());
}
