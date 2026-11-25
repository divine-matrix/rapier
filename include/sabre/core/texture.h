#pragma once

#include "sabre/core/core.h"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <cstdint>
#include <vector>

namespace sabre::core {

struct sabre_RenderRect {
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

struct sabre_ProcTextureObject {
  uint32_t width = 0;
  uint32_t height = 0;
  sabre_TextureFilterMode filterMode = sabre_TextureFilterMode::NEAREST;
  std::vector<uint32_t> textureBuffer;
  sabre_RenderRect renderRect{};
  GLuint glTextureId = 0;
  bool isDirty = true;
};

sabre_ProcTextureObject sabre_CreateProcTextureObject(sabre_GlobalContext &gctx,
                                                      uint32_t width,
                                                      uint32_t height);

sabre_ProcTextureObject
sabre_CreateProcTextureObject(sabre_GlobalContext &gctx, uint32_t width,
                              uint32_t height,
                              sabre_TextureFilterMode filterMode);

void setPixelColor(sabre_ProcTextureObject &procTex, uint32_t xCoord,
                   uint32_t yCoord, uint8_t redVal, uint8_t greenVal,
                   uint8_t blueVal, uint8_t alphaVal = 255);

void uploadTextureBuffer(sabre_ProcTextureObject &procTex);

// FIX: Passed sabre_GlobalContext so shaders can be loaded via sabre_LoadFile
void sabre_RenderProcTextureObject(const sabre_GlobalContext &gctx,
                                   SDL_Window *window,
                                   sabre_ProcTextureObject &procTex);

void shutdownProcTextureRenderer();

void destroyProcTextureObject(sabre_ProcTextureObject &procTex);

} // namespace sabre::core
