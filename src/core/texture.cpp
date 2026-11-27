#include "sabre/core/texture.h"
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// Main namespace
namespace sabre::core {
// Private namespace, so no other files can use stuff here
namespace {

// FIX: Default shader paths resolved relative to gctx.project_dir by
// LoadFile
constexpr const char *kDefaultVertShaderPath = "shaders/proc_texture.vert";
constexpr const char *kDefaultFragShaderPath = "shaders/proc_texture.frag";

// Defines container for shared OpenGL quad renderer objects
struct CoreQuadRenderer {
  bool initialized = false;
  GLuint shaderProgram = 0;
  GLuint vao = 0;
  GLuint vbo = 0;
  // -1 is not found
  GLint textureUniformLoc = -1;
};

// creates core instance of CoreQuadRenderer
CoreQuadRenderer gQuadRenderer{};

// Converts sabre TextureFilterMode to glFilterMode
GLenum toGlFilterMode(TextureFilterMode mode) {
  switch (mode) {
  case TextureFilterMode::LINEAR:
  case TextureFilterMode::BILINEAR:
    return GL_LINEAR;
  case TextureFilterMode::NEAREST:
  default:
    return GL_NEAREST;
  }
}

// Compiles shader from path
GLuint compileShaderFromFile(const GlobalContext &gctx, GLenum shaderType,
                             const char *shaderPath) {
  // FIX: Load shader source using LoadFile
  const std::string sourceStr = LoadFile(gctx, shaderPath);
  // Checks whether loading returned nothing, and returns error if so
  if (sourceStr.empty()) {
    std::cerr << "Failed to load shader file via LoadFile: " << shaderPath
              << "\n";
    return 0;
  }
  // Shader source text
  const char *sourcePtr = sourceStr.c_str();
  // gets shader id for this shader
  const GLuint shaderId = glCreateShader(shaderType);
  // attaches shader source to above shader id
  glShaderSource(shaderId, 1, &sourcePtr, nullptr);
  // compiles shader source
  glCompileShader(shaderId);

  // prepare variable for whether it is compiled or not
  GLint compiled = GL_FALSE;
  // gets if it is compiled and changes variable by reference
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compiled);
  // checks if compiled is true, if it isn't, return error message
  if (compiled == GL_FALSE) {
    GLint logLength = 0;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<size_t>(logLength > 1 ? logLength : 1), '\0');
    glGetShaderInfoLog(shaderId, logLength, nullptr, log.data());
    std::cerr << "Shader compile failed (" << shaderPath << "): " << log
              << "\n";
    // makes the shader object not leak
    glDeleteShader(shaderId);
    return 0;
  }
  // on success, return shader id
  return shaderId;
}

// Initializes the shared quad renderer if it has not yet been initialized
bool ensureCoreQuadRenderer(const GlobalContext &gctx) {
  // cancels if it is already initialized
  if (gQuadRenderer.initialized) {
    return true;
  }

  // compiles vertex shader, and fails if compilation fails
  const GLuint vertShader =
      compileShaderFromFile(gctx, GL_VERTEX_SHADER, kDefaultVertShaderPath);
  if (vertShader == 0) {
    return false;
  }
  // compiles fragment shader, fails if compilation fails
  const GLuint fragShader =
      compileShaderFromFile(gctx, GL_FRAGMENT_SHADER, kDefaultFragShaderPath);
  if (fragShader == 0) {
    glDeleteShader(vertShader);
    return false;
  }

  // creates program with ID program
  const GLuint program = glCreateProgram();
  // attaches shaders to program
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  // binds attributes?
  glBindAttribLocation(program, 0, "aPos");
  glBindAttribLocation(program, 1, "aTexCoord");
  // links attached shaders into executable gpu program
  glLinkProgram(program);

  // deletes shaders so they don't leak
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);

  // does same as in ensureCoreQuadRenderer, checks if program linked, if not,
  // fails
  GLint linked = GL_FALSE;
  glGetProgramiv(program, GL_LINK_STATUS, &linked);
  if (linked == GL_FALSE) {
    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<size_t>(logLength > 1 ? logLength : 1), '\0');
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    std::cerr << "Shader link failed: " << log << "\n";
    // frees failed program object
    glDeleteProgram(program);
    // quits function
    return false;
  }

  // initializes ids for vao and vbo
  GLuint vao = 0;
  GLuint vbo = 0;
  // generates vertex array into vao
  glGenVertexArrays(1, &vao);
  // generates vertex buffer into vbo
  glGenBuffers(1, &vbo);

  // binds vertex array
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

void initializeTextureObject(TextureRect &procTex, uint32_t width,
                             uint32_t height, TextureFilterMode filterMode) {
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

TextureRect CreateTextureRect(GlobalContext &gctx, TextureRectSystem &sys,
                              uint32_t width, uint32_t height) {
  // Initializes procTex
  TextureRect procTex;
  initializeTextureObject(procTex, width, height, gctx.defaultFilterMode);
  // Adds procTex's data to TextureRectSystem
  sys.destwidth.push_back(procTex.renderRect.width);
  sys.destheight.push_back(procTex.renderRect.height);
  sys.destx.push_back(procTex.renderRect.x);
  sys.desty.push_back(procTex.renderRect.y);
  sys.filterMode.push_back(procTex.filterMode);
  sys.glTextureId.push_back(procTex.glTextureId);
  sys.width.push_back(procTex.width);
  sys.height.push_back(procTex.height);
  sys.height.push_back(procTex.height);
  sys.isDirty.push_back(procTex.isDirty);
  return procTex;
}

// Deprecated
// void setPixelColor(TextureRect &procTex, uint32_t xCoord, uint32_t yCoord,
//                    uint8_t redVal, uint8_t greenVal, uint8_t blueVal,
//                    uint8_t alphaVal) {
//   if (xCoord >= procTex.width || yCoord >= procTex.height) {
//     return;
//   }

//   const uint32_t packedColor = (static_cast<uint32_t>(alphaVal) << 24) |
//                                (static_cast<uint32_t>(blueVal) << 16) |
//                                (static_cast<uint32_t>(greenVal) << 8) |
//                                static_cast<uint32_t>(redVal);

//   procTex.textureBuffer[static_cast<size_t>(yCoord) * procTex.width + xCoord]
//   =
//       packedColor;
//   procTex.isDirty = true;
// }

void setTextureBuffer(TextureRectSystem &sys, std::vector<uint32_t> buffer,
                      uint32_t idx) {
  // Checks if the buffer provided is the same size as the original buffer
  if (buffer.size() == sys.buffers[idx].buffer.size()) {
    sys.buffers[idx].buffer = buffer;
    sys.isDirty[idx] = true;
    uploadTextureBuffer(sys, idx);
  } else { // throw error if not
    std::cerr << "Buffer of incorrect size provided to setTextureBuffer. "
                 "Original buffer size: "
              << sys.buffers[idx].buffer.size()
              << "B, provided buffer size: " << buffer.size() << std::endl;
    return;
  }
}

void uploadTextureBuffer(TextureRectSystem &sys, uint32_t id) {
  if (!sys.isDirty[id] || sys.glTextureId[id] == 0 ||
      sys.buffers[id].buffer.empty()) {
    return;
  }

  glBindTexture(GL_TEXTURE_2D, sys.glTextureId[id]);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<GLsizei>(sys.width[id]),
                  static_cast<GLsizei>(sys.height[id]), GL_RGBA,
                  GL_UNSIGNED_BYTE, sys.buffers[id].buffer.data());
  glBindTexture(GL_TEXTURE_2D, 0);
  sys.isDirty[id] = false;
}

void RenderProcTextureObject(const GlobalContext &gctx, SDL_Window *window,
                             TextureRectSystem &sys, uint32_t id) {
  if (!window || sys.width[id] == 0 || sys.height[id] == 0 ||
      sys.glTextureId[id] == 0) {
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

  uploadTextureBuffer(sys, id);

  const float screenWidth = static_cast<float>(pixelW);
  const float screenHeight = static_cast<float>(pixelH);

  const float leftNdc = (sys.destx[id] / screenWidth) * 2.0f - 1.0f;
  const float rightNdc =
      ((sys.destx[id] + sys.destwidth[id]) / screenWidth) * 2.0f - 1.0f;
  const float topNdc = 1.0f - (sys.destheight[id] / screenHeight) * 2.0f;
  const float bottomNdc =
      1.0f - ((sys.desty[id] + sys.destheight[id]) / screenHeight) * 2.0f;

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
  glBindTexture(GL_TEXTURE_2D, sys.glTextureId[id]);
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

void destroyProcTextureObject(TextureRect &procTex) {
  if (procTex.glTextureId != 0) {
    glDeleteTextures(1, &procTex.glTextureId);
    procTex.glTextureId = 0;
  }
  procTex.textureBuffer.clear();
  procTex.width = 0;
  procTex.height = 0;
  procTex.isDirty = false;
}

uint32_t getFreeTextureHandle(TextureRectSystem &sys) {
  // holds free handle
  uint32_t id = kInvalidHandle;

  // get any free, reusable handles, before issuing a brand new one
  if (!sys.freeHandles.empty()) {
  }
}

} // namespace sabre::core
