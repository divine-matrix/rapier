#pragma once
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"
#include "sabre/core/texture.h"
#include "sabre/core/window.h"
#include <SDL3/SDL.h>
#include <cstdint>

namespace sabre::tools::debug {

struct PerfStats {
  double frameMs = 0.0;
  double cpuGenMs = 0.0;
  double renderTexMs = 0.0;
  double imguiMs = 0.0;
};

struct DebugDrawState {
  bool animatePattern = true;
};

void draw(sabre::core::GlobalContext &gctx,
          sabre::core::ProcTextureObject &procTex,
          const PerfStats &perfStats, DebugDrawState &drawState);

// FIX: Convert SDL performance counter ticks to milliseconds
inline double ticksToMs(uint64_t startTicks, uint64_t endTicks) {
  const uint64_t freqTicks = SDL_GetPerformanceFrequency();
  if (freqTicks == 0 || endTicks <= startTicks) {
    return 0.0;
  }
  return (static_cast<double>(endTicks - startTicks) * 1000.0) /
         static_cast<double>(freqTicks);
}

void initialize(sabre::core::Window window, SDL_GLContext glContext);

} // namespace sabre::tools::debug
