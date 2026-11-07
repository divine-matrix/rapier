#include "sabre/core/texture.h"

namespace sabre::core {

sabre_ProcTextureObject sabre_CreateProcTextureObject(sabre_GlobalContext &gctx,
                                                      uint32_t width,
                                                      uint32_t height) {

  // temp object to return
  sabre_ProcTextureObject obj;

  obj.width = width;
  obj.height = height;
  obj.filterMode = gctx.defaultFilterMode;
  obj.textureBuffer.resize(width * height, 0);

  return obj;
}

// NOTE: Bad formatting here
sabre_ProcTextureObject
sabre_CreateProcTextureObject(sabre_GlobalContext &gctx, uint32_t width,
                              uint32_t height,
                              sabre_TextureFilterMode filterMode) {

  // temp object to return
  sabre_ProcTextureObject obj;

  obj.width = width;
  obj.height = height;
  obj.filterMode = filterMode;
  obj.textureBuffer.resize(width * height, 0);

  return obj;
}

} // namespace sabre::core
