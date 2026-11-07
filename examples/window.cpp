#include "sabre/core/window.h"
#include "sabre/core/core.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <iostream>

int main() {
  sabre::core::sabre_GlobalContext gctx;
  gctx.sdl_flags = SDL_INIT_VIDEO;
  sabre::core::sabre_Init(gctx);

  sabre::core::WindowSystemData winsys;
  sabre::core::sabre_CreateWindow(winsys, "Example Window", 800, 600, 0);

  if (winsys.window_pool.empty() || winsys.window_pool[0].handle == nullptr) {
    std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  bool running = true;
  Uint64 start = SDL_GetTicks();
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_EVENT_QUIT)
        running = false;
    }
    SDL_Delay(10);
  }

  sabre::core::sabre_DestroyWindow(winsys, 0);
  sabre::core::sabre_Quit();
}
