#include <OpenGL/gl3.h>
#include <string>

namespace sabre::core {

struct VertexShader {
  std::string *content;
};

struct FragmentShader {
  std::string *content;
};

struct PackagedShader {
  VertexShader vertexShader;
  FragmentShader fragmentShader;
};

GLuint LoadShader(const std::string &path);

} // namespace sabre::core
