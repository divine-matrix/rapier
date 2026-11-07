#include "sabre/core/core.h"
#include <cstdint>
#include <vector>

namespace sabre::core {

// Procedural Texture Object data and functions
struct sabre_ProcTextureObject {
  uint32_t width, height;
  sabre::core::sabre_TextureFilterMode filterMode;
  std::vector<uint32_t> textureBuffer;
};

// Create ProcTextureObject functions, and associated overloads

sabre_ProcTextureObject sabre_CreateProcTextureObject(sabre_GlobalContext &gctx,
                                                      uint32_t width,
                                                      uint32_t height);

// NOTE: nvim has bad formatting here
sabre_ProcTextureObject
sabre_CreateProcTextureObject(sabre_GlobalContext &gctx, uint32_t width,
                              uint32_t height,
                              sabre_TextureFilterMode filterMode);
} // namespace sabre::core
