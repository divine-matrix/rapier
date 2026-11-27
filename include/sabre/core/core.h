#pragma once

#include <SDL3/SDL_init.h>
#include <string>
namespace sabre::core {

// Filter mode for scaling textures
enum TextureFilterMode { NEAREST, LINEAR, BILINEAR };

// Sabre global context data and functions
struct GlobalContext {
  TextureFilterMode defaultFilterMode;
  // initialisation values
  SDL_InitFlags sdl_flags;
  // file loading settings
  std::string project_dir = "./";
};

std::string LoadFile(GlobalContext gctx, const std::string &path);

bool Init(GlobalContext &gctx);
void Quit();

} // namespace sabre::core
