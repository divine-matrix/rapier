#include "sabre/core/core.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace sabre::core {

std::string sabre_LoadFile(sabre_GlobalContext gctx, const std::string &path) {
  std::filesystem::path p(gctx.project_dir + path);
  auto size = std::filesystem::file_size(p);
  std::string content(size, '\0');
  std::ifstream file(p, std::ios::binary);
  file.read(content.data(), size);
  return content;
}

bool sabre_Init(sabre_GlobalContext &gctx) {
  if (!SDL_Init(gctx.sdl_flags)) {
    std::cout
        << "[sabre_engine - sabre_init] [error] - Failed to initialise SDL"
        << std::endl;
    std::cerr << "Failed to initialise SDL";
    return false;
  }
  return true;
}

void sabre_Quit() { SDL_Quit(); }

} // namespace sabre::core
