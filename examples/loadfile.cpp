#include "sabre/core/core.h"
#include <iostream>

int main() {
  sabre::core::GlobalContext gctx;
  gctx.project_dir = "/Users/blockgenie/Documents/_divinematrix/rapier/"
                     "examples/";
  std::cout << sabre::core::LoadFile(gctx, "example.txt") << std::endl;
  return 0;
}
