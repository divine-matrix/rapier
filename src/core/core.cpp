#include "sabre/core/core.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace sabre::core {

std::string LoadFile(GlobalContext gctx, const std::string &path) {
  std::filesystem::path p(gctx.project_dir + path);
  auto size = std::filesystem::file_size(p);
  std::string content(size, '\0');
  std::ifstream file(p, std::ios::binary);
  file.read(content.data(), size);
  return content;
}

bool Init(GlobalContext &gctx) {
  if (!SDL_Init(gctx.sdl_flags)) {
    std::cout
        << "[engine - init] [error] - Failed to initialise SDL"
        << std::endl;
    std::cerr << "Failed to initialise SDL";
    return false;
  }
  return true;
}

void Quit() { SDL_Quit(); }

} // namespace sabre::core
