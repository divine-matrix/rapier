#include "sabre/sabre.h"
#include <iostream>

int main() {
  std::cout << "Sabre Version: " << sabre::version() << std::endl;
  return 0;
}
