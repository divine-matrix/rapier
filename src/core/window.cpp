#include "sabre/core/window.h"
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>

namespace sabre::core {

uint32_t WindowSystemData::sabre_GetWindowIndexBySdlId(uint32_t sdl_id) {
  for (size_t i = 0; i < window_pool.size(); ++i) {
    if (sdl_id == window_pool[i].sdl_id) {
      return i;
    }
  }
  return -1;
}

void sabre_CreateWindow(WindowSystemData &window_system, const char title[64],
                        uint32_t a_width, uint32_t a_height,
                        uint64_t custom_flags) {
  Window win;

  win.width = a_width;
  win.height = a_height;

  SDL_strlcpy(win.title, title, sizeof(win.title));

  uint64_t final_flags = custom_flags | SDL_WINDOW_OPENGL;
  win.handle = SDL_CreateWindow(win.title, win.width, win.height, final_flags);
  if (win.handle != nullptr) {
    win.sdl_id = SDL_GetWindowID(win.handle);
    window_system.window_pool.push_back(win);
  } else {
    SDL_Log("Failed to create window: %s", SDL_GetError());
  }
}

void sabre_DestroyWindow(WindowSystemData &window_system, WindowID win_idx) {
  if (win_idx >= window_system.window_pool.size())
    return;
  Window &win = window_system.window_pool[win_idx];

  if (win.handle != nullptr) {
    SDL_DestroyWindow(win.handle);
  }

  if (win_idx < window_system.window_pool.size() - 1) {
    window_system.window_pool[win_idx] = window_system.window_pool.back();
  }

  window_system.window_pool.pop_back();
}

} // namespace sabre::core
