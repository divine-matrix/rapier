#include "sabre/core/core.h"
#include <string>
namespace sabre::core {
std::string sabre_LoadShader(const std::string &path) {
  return sabre_LoadFile(path);
}
} // namespace sabre::core
