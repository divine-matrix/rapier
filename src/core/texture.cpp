#include "sabre/core/texture.h"
#include <iostream>
#include <string>

namespace sabre::core {

namespace {

// FIX: Default shader paths resolved relative to gctx.project_dir by
// sabre_LoadFile
constexpr const char *kDefaultVertShaderPath = "shaders/proc_texture.vert";
constexpr const char *kDefaultFragShaderPath = "shaders/proc_texture.frag";

struct CoreQuadRenderer {
  bool initialized = false;
  GLuint shaderProgram = 0;
  GLuint vao = 0;
  GLuint vbo = 0;
  GLint textureUniformLoc = -1;
};

CoreQuadRenderer gQuadRenderer{};

GLenum toGlFilterMode(sabre_TextureFilterMode mode) {
  switch (mode) {
  case sabre_TextureFilterMode::LINEAR:
  case sabre_TextureFilterMode::BILINEAR:
    return GL_LINEAR;
  case sabre_TextureFilterMode::NEAREST:
  default:
    return GL_NEAREST;
  }
}

GLuint compileShaderFromFile(const sabre_GlobalContext &gctx, GLenum shaderType,
                             const char *shaderPath) {
  // FIX: Load shader source using sabre_LoadFile
  const std::string sourceStr = sabre_LoadFile(gctx, shaderPath);
  if (sourceStr.empty()) {
    std::cerr << "Failed to load shader file via sabre_LoadFile: " << shaderPath
              << "\n";
    return 0;
  }

  const char *sourcePtr = sourceStr.c_str();
  const GLuint shaderId = glCreateShader(shaderType);
  glShaderSource(shaderId, 1, &sourcePtr, nullptr);
  glCompileShader(shaderId);

  GLint compiled = GL_FALSE;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
  if (compiled == GL_FALSE) {
    GLint logLength = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<size_t>(logLength > 1 ? logLength : 1), '\0');
    glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
    std::cerr << "Shader compile failed (" << shaderPath << "): " << log
              << "\n";
    glDeleteShader(shaderId);
    return 0;
  }

  return shaderId;
}

bool ensureCoreQuadRenderer(const sabre_GlobalContext &gctx) {
  if (gQuadRenderer.initialized) {
    return true;
  }

  const GLuint vertShader =
      compileShaderFromFile(gctx, GL_VERTEX_SHADER, kDefaultVertShaderPath);
  if (vertShader == 0) {
    return false;
  }

  const GLuint fragShader =
      compileShaderFromFile(gctx, GL_FRAGMENT_SHADER, kDefaultFragShaderPath);
  if (fragShader == 0) {
    glDeleteShader(vertShader);
    return false;
  }

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glBindAttribLocation(program, 0, "aPos");
  glBindAttribLocation(program, 1, "aTexCoord");
  glLinkProgram(program);

  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  GLint linked = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<size_t>(logLength > 1 ? logLength : 1), '\0');
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    std::cerr << "Shader link failed: " << log << "\n";
    glDeleteProgram(program);
    return false;
  }

  GLuint vao = 0;
  GLuint vbo = 0;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr,
               GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                        reinterpret_cast<const void *>(0));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                        reinterpret_cast<const void *>(sizeof(float) * 2));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  gQuadRenderer.shaderProgram = program;
  gQuadRenderer.vao = vao;
  gQuadRenderer.vbo = vbo;
  gQuadRenderer.textureUniformLoc = glGetUniformLocation(program, "uTexture");
  gQuadRenderer.initialized = true;
  return true;
}

void initializeTextureObject(sabre_ProcTextureObject &procTex, uint32_t width,
                             uint32_t height,
                             sabre_TextureFilterMode filterMode) {
  procTex.width = width;
  procTex.height = height;
  procTex.filterMode = filterMode;
  procTex.textureBuffer.assign(static_cast<size_t>(width) * height,
                               0xFF000000u);
  procTex.renderRect = {0.0f, 0.0f, static_cast<float>(width),
                        static_cast<float>(height)};
  procTex.isDirty = true;

  if (procTex.glTextureId == 0) {
    glGenTextures(1, &procTex.glTextureId);
  }

  const GLenum glFilter = toGlFilterMode(filterMode);

  glBindTexture(GL_TEXTURE_2D, procTex.glTextureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glFilter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, static_cast<GLsizei>(width),
               static_cast<GLsizei>(height), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               procTex.textureBuffer.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace

sabre_ProcTextureObject sabre_CreateProcTextureObject(sabre_GlobalContext &gctx,
                                                      uint32_t width,
                                                      uint32_t height) {
  sabre_ProcTextureObject procTex;
  initializeTextureObject(procTex, width, height, gctx.defaultFilterMode);
  return procTex;
}

sabre_ProcTextureObject
sabre_CreateProcTextureObject(sabre_GlobalContext &gctx, uint32_t width,
                              uint32_t height,
                              sabre_TextureFilterMode filterMode) {
  (void)gctx;
  sabre_ProcTextureObject procTex;
  initializeTextureObject(procTex, width, height, filterMode);
  return procTex;
}

void setPixelColor(sabre_ProcTextureObject &procTex, uint32_t xCoord,
                   uint32_t yCoord, uint8_t redVal, uint8_t greenVal,
                   uint8_t blueVal, uint8_t alphaVal) {
  if (xCoord >= procTex.width || yCoord >= procTex.height) {
    return;
  }

  const uint32_t packedColor = (static_cast<uint32_t>(alphaVal) << 24) |
                               (static_cast<uint32_t>(blueVal) << 16) |
                               (static_cast<uint32_t>(greenVal) << 8) |
                               static_cast<uint32_t>(redVal);

  procTex.textureBuffer[static_cast<size_t>(yCoord) * procTex.width + xCoord] =
      packedColor;
  procTex.isDirty = true;
}

void uploadTextureBuffer(sabre_ProcTextureObject &procTex) {
  if (!procTex.isDirty || procTex.glTextureId == 0 ||
      procTex.textureBuffer.empty()) {
    return;
  }

  glBindTexture(GL_TEXTURE_2D, procTex.glTextureId);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(procTex.width),
                  static_cast<GLsizei>(procTex.height), GL_RGBA,
                  GL_UNSIGNED_BYTE, procTex.textureBuffer.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  procTex.isDirty = false;
}

void sabre_RenderProcTextureObject(const sabre_GlobalContext &gctx,
                                   SDL_Window *window,
                                   sabre_ProcTextureObject &procTex) {
  if (!window || procTex.width == 0 || procTex.height == 0 ||
      procTex.glTextureId == 0) {
    return;
  }

  int pixelW = 0;
  int pixelH = 0;
  SDL_GetWindowSizeInPixels(window, &pixelW, &pixelH);
  if (pixelW <= 0 || pixelH <= 0) {
    return;
  }

  if (!ensureCoreQuadRenderer(gctx)) {
    return;
  }

  uploadTextureBuffer(procTex);

  const float screenWidth = static_cast<float>(pixelW);
  const float screenHeight = static_cast<float>(pixelH);

  const float leftNdc = (procTex.renderRect.x / screenWidth) * 2.0f - 1.0f;
  const float rightNdc =
      ((procTex.renderRect.x + procTex.renderRect.width) / screenWidth) * 2.0f -
      1.0f;
  const float topNdc = 1.0f - (procTex.renderRect.y / screenHeight) * 2.0f;
  const float bottomNdc =
      1.0f -
      ((procTex.renderRect.y + procTex.renderRect.height) / screenHeight) *
          2.0f;

  const float vertices[6 * 4] = {
      leftNdc, topNdc, 0.0f,     0.0f,      rightNdc, topNdc,
      1.0f,    0.0f,   rightNdc, bottomNdc, 1.0f,     1.0f,

      leftNdc, topNdc, 0.0f,     0.0f,      rightNdc, bottomNdc,
      1.0f,    1.0f,   leftNdc,  bottomNdc, 0.0f,     1.0f};

  glViewport(0, 0, pixelW, pixelH);

  glBindBuffer(GL_ARRAY_BUFFER, gQuadRenderer.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glUseProgram(gQuadRenderer.shaderProgram);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, procTex.glTextureId);
  if (gQuadRenderer.textureUniformLoc >= 0) {
    glUniform1i(gQuadRenderer.textureUniformLoc, 0);
  }

  glBindVertexArray(gQuadRenderer.vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);
}

void shutdownProcTextureRenderer() {
  if (gQuadRenderer.vbo != 0) {
    glDeleteBuffers(1, &gQuadRenderer.vbo);
    gQuadRenderer.vbo = 0;
  }
  if (gQuadRenderer.vao != 0) {
    glDeleteVertexArrays(1, &gQuadRenderer.vao);
    gQuadRenderer.vao = 0;
  }
  if (gQuadRenderer.shaderProgram != 0) {
    glDeleteProgram(gQuadRenderer.shaderProgram);
    gQuadRenderer.shaderProgram = 0;
  }
  gQuadRenderer.textureUniformLoc = -1;
  gQuadRenderer.initialized = false;
}

void destroyProcTextureObject(sabre_ProcTextureObject &procTex) {
  if (procTex.glTextureId != 0) {
    glDeleteTextures(1, &procTex.glTextureId);
    procTex.glTextureId = 0;
  }
  procTex.textureBuffer.clear();
  procTex.width = 0;
  procTex.height = 0;
  procTex.isDirty = false;
}

} // namespace sabre::core
