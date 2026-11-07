#include "sabre/core/core.h"
#include <OpenGL/gl3.h>
#include <OpenGL/gltypes.h>
#include <string>
namespace sabre::core {
GLuint sabre_LoadShader(sabre_GlobalContext gctx, const std::string &path,
                        GLenum type) {
  std::string src = sabre_LoadFile(gctx, path);
  const char *source = src.c_str();
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &source, nullptr);
  glCompileShader(s);

  GLint ok;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(s, 512, nullptr, log);
    fprintf(stderr, "Shader compile error: %s\n", log);
  }
  return s;
}
} // namespace sabre::core
