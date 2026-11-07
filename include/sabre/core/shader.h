#include <OpenGL/gl3.h>
#include <string>

namespace sabre::core {

struct sabre_VertexShader {
  std::string *content;
};

struct sabre_FragmentShader {
  std::string *content;
};

struct sabre_PackagedShader {
  sabre_VertexShader vertexShader;
  sabre_FragmentShader fragmentShader;
};

GLuint sabre_LoadShader(const std::string &path);

} // namespace sabre::core
