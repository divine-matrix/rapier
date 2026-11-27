#pragma once

#include "sabre/core/core.h"
#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

namespace sabre::core {

struct TextureBuffer {
  std::vector<uint32_t> buffer;
};

constexpr uint32_t kInvalidSlot = std::numeric_limits<uint32_t>::max();
constexpr uint32_t kInvalidHandle = std::numeric_limits<uint32_t>::max();

struct TextureRectSystem {
  // Handle helper values
  uint32_t currentHandle = 0;
  std::vector<uint32_t> freeHandles;
  std::vector<uint32_t> handleToSlot;
  std::vector<uint32_t> slotToHandle;
  // NOTE: Basic metadata for texture
  std::vector<float> destx;
  std::vector<float> desty;
  std::vector<float> destwidth;
  std::vector<float> destheight;
  std::vector<uint32_t> width;
  std::vector<uint32_t> height;
  std::vector<TextureFilterMode> filterMode;
  std::vector<bool> isDirty;
  // NOTE: Texture buffer stuff
  std::vector<TextureBuffer> buffers;
  // Currently unused
  // std::vector<uint32_t> bufferIndex;
  std::vector<GLuint> glTextureId;
};
struct RenderRect {
  float x = 0.0f;
  float y = 0.0f;
  float width = 0.0f;
  float height = 0.0f;
};

// Deprecated. DO NOT USE
struct TextureRect {
  uint32_t width = 0;
  uint32_t height = 0;
  TextureFilterMode filterMode = TextureFilterMode::NEAREST;
  std::vector<uint32_t> textureBuffer;
  RenderRect renderRect{};
  GLuint glTextureId = 0;
  bool isDirty = true;
  uint32_t systemId;
};

// Gets a free texture handle/id that can be used to access textures in a
// TextureRectSystem
uint32_t getFreeTextureHandle(TextureRectSystem &sys);

TextureRect CreateProcTextureObject(GlobalContext &gctx, uint32_t width,
                                    uint32_t height);

TextureRect CreateProcTextureObject(GlobalContext &gctx, uint32_t width,
                                    uint32_t height,
                                    TextureFilterMode filterMode);

TextureRect getData(uint32_t idx);

void setTextureBuffer(TextureRectSystem &sys, std::vector<uint32_t> buffer,
                      uint32_t idx);

void uploadTextureBuffer(TextureRectSystem &sys, uint32_t id);

// FIX: Passed GlobalContext so shaders can be loaded via LoadFile
void RenderProcTextureObject(const GlobalContext &gctx, SDL_Window *window,
                             TextureRect &procTex);

void shutdownProcTextureRenderer();

void destroyProcTextureObject(TextureRect &procTex);

} // namespace sabre::core
