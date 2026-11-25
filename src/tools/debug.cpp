#include "sabre/tools/debug.h"
#include "imgui.h"
#include <array>

namespace sabre::tools::debug {

void draw(sabre::core::sabre_GlobalContext &gctx,
          sabre::core::sabre_ProcTextureObject &procTex,
          const sabre_PerfStats &perfStats, DebugDrawState &drawState) {
  ImGui::Begin("Sabre Debug");

  const double fpsVal =
      (perfStats.frameMs > 0.0) ? (1000.0 / perfStats.frameMs) : 0.0;

  // FIX: Estimate theoretical max FPS from active stage time (excludes VSync
  // wait)
  const double activeWorkMs =
      perfStats.cpuGenMs + perfStats.renderTexMs + perfStats.imguiMs;
  const double theoMaxFps =
      (activeWorkMs > 0.0) ? (1000.0 / activeWorkMs) : 0.0;

  ImGui::Text("FPS: %.1f (%.2f ms)", fpsVal, perfStats.frameMs);
  ImGui::Text("Theo. Max FPS: %.1f (%.3f ms work)", theoMaxFps, activeWorkMs);
  ImGui::Separator();

  ImGui::Text("CPU Pixel Gen: %.3f ms", perfStats.cpuGenMs);
  ImGui::Text("ProcTex Render: %.3f ms", perfStats.renderTexMs);
  ImGui::Text("ImGui UI: %.3f ms", perfStats.imguiMs);

  ImGui::Separator();
  ImGui::Checkbox("Animate Pattern", &drawState.animatePattern);

  std::array<float, 2> rectPos = {procTex.renderRect.x, procTex.renderRect.y};
  if (ImGui::DragFloat2("Rect Position", rectPos.data(), 1.0f)) {
    procTex.renderRect.x = rectPos[0];
    procTex.renderRect.y = rectPos[1];
  }

  std::array<float, 2> rectSize = {procTex.renderRect.width,
                                   procTex.renderRect.height};
  if (ImGui::DragFloat2("Rect Size", rectSize.data(), 1.0f, 1.0f, 4096.0f)) {
    procTex.renderRect.width = rectSize[0];
    procTex.renderRect.height = rectSize[1];
  }

  const char *filterOptions[] = {"NEAREST", "LINEAR", "BILINEAR"};
  int filterIndex = static_cast<int>(procTex.filterMode);
  if (ImGui::Combo("Filter Mode", &filterIndex, filterOptions,
                   IM_ARRAYSIZE(filterOptions))) {
    procTex.filterMode =
        static_cast<sabre::core::sabre_TextureFilterMode>(filterIndex);
    if (procTex.glTextureId != 0) {
      GLenum glFilter = GL_NEAREST;
      if (procTex.filterMode == sabre::core::sabre_TextureFilterMode::LINEAR ||
          procTex.filterMode ==
              sabre::core::sabre_TextureFilterMode::BILINEAR) {
        glFilter = GL_LINEAR;
      }
      glBindTexture(GL_TEXTURE_2D, procTex.glTextureId);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }

  ImGui::Text("Texture Size: %u x %u", procTex.width, procTex.height);
  ImGui::Text("Dirty Flag: %s", procTex.isDirty ? "true" : "false");
  ImGui::Text("Project Dir: %s", gctx.project_dir.c_str());

  ImGui::End();
}

} // namespace sabre::tools::debug
