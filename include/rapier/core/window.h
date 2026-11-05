#include <cstdint>
#include <vector>

using WindowID = uint32_t;

#include <SDL3/SDL.h> // Or <SDL2/SDL.h>
#include <cstdint>

namespace rapier::core {

struct Window {
  // Platform Identifiers
  SDL_Window *handle = nullptr;
  std::uint32_t sdl_id = 0; // Added: Derived via SDL_GetWindowID(handle)

  // Configuration / Specifications
  std::uint32_t width = 800;
  std::uint32_t height = 600;
  bool is_fullscreen = false;
  bool is_minimized = false;
  char title[64] = "Window";

  // State Tracking / Input
  bool close_event = false;
  int mouse_x = 0;
  int mouse_y = 0;
};

struct WindowSystemData {
  std::vector<Window> window_pool;

  uint32_t Rapier_GetWindowIndexBySdlId(WindowID id);
};

void Rapier_CreateWindow(WindowSystemData &window_system, const char title[64],
                         uint32_t width, uint32_t height,
                         uint64_t custom_flags);

void Rapier_DestroyWindow(WindowSystemData &window_system, WindowID win_id);

} // namespace rapier::core
