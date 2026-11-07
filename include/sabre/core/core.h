#include <SDL3/SDL_init.h>
#include <string>
namespace sabre::core {

// Filter mode for scaling textures
enum sabre_TextureFilterMode { NEAREST, LINEAR, BILINEAR };

// Sabre global context data and functions
struct sabre_GlobalContext {
  sabre_TextureFilterMode defaultFilterMode;
  // initialisation values
  SDL_InitFlags sdl_flags;
  // file loading settings
  std::string project_dir = "./";
};

std::string sabre_LoadFile(sabre_GlobalContext gctx, const std::string &path);

bool sabre_Init(sabre_GlobalContext &gctx);
void sabre_Quit();

} // namespace sabre::core
