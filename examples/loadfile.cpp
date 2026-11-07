#include "sabre/core/core.h"
#include <iostream>

int main() {
  std::cout << sabre::core::sabre_LoadFile(
                   "/Users/blockgenie/Documents/_divinematrix/rapier/examples/"
                   "example.txt")
            << std::endl;
  return 0;
}
