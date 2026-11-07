#include "sabre/core/window.h"
#include <SDL3/SDL.h>
#include <iostream>

int main() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

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
  SDL_Quit();
}
