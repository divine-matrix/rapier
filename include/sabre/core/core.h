#include <string>
namespace sabre::core {

std::string sabre_LoadFile(const std::string &path);

// Filter mode for scaling textures
enum sabre_TextureFilterMode { NEAREST, LINEAR, BILINEAR };

// Sabre global context data and functions
struct sabre_GlobalContext {
  sabre_TextureFilterMode defaultFilterMode;
};

} // namespace sabre::core
