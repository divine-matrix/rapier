#include "sabre/core/core.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>

namespace sabre::core {

std::string sabre_LoadFile(const std::string &path) {
  std::filesystem::path p(path);
  auto size = std::filesystem::file_size(p);
  std::string content(size, '\0');
  std::ifstream file(p, std::ios::binary);
  file.read(content.data(), size);
  return content;
}

} // namespace sabre::core
