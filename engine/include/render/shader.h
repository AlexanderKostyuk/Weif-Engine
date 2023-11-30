#ifndef WE_REDNER_SHADER_H
#define WE_REDNER_SHADER_H
#include <GL/gl3w.h>

namespace WE::Render {

class Shader {
public:
  Shader() = delete;
  Shader(const Shader &shader) = delete;
  Shader(const GLchar *path, const GLenum shaderType);
  ~Shader();

  inline GLuint GetShaderRef() const { return shader_reference; }

private:
  GLuint shader_reference;
};
} // namespace WE::Render

#endif // !SHADER_H
