#include "render/shader.h"

#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#include <cstdio>
#include <string>

#include "utils.h"

namespace {

const char *GetShaderTypeName(GLenum shader_type) {
  switch (shader_type) {
  case GL_VERTEX_SHADER:
    return "VERTEX";
  case GL_FRAGMENT_SHADER:
    return "FRAGMENT";
  case GL_COMPUTE_SHADER:
    return "COMPUTE";
  default:
    return "UNKNOWN";
  }
}

void HandleErrors(GLuint shader, GLenum shader_type) {

  int success;
  char info_log[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, info_log);
    const char *shader_type_name = GetShaderTypeName(shader_type);
    printf("ERROR::SHADER::%s::COMPILATION_FAILED\n%s\n", shader_type_name,
           info_log);
  }
}
} // namespace

namespace WE::Render {
Shader::Shader(const char *path, GLenum shader_type) {

  printf("Building shader \"%s\", of type %s\n", path,
         GetShaderTypeName(shader_type));

  std::string shader_src = Utils::ReadFile(path);
  const char *shader_src_c_str = shader_src.c_str();

  shader_reference = glCreateShader(shader_type);

  glShaderSource(shader_reference, 1, &shader_src_c_str, nullptr);
  glCompileShader(shader_reference);

  HandleErrors(shader_reference, shader_type);
  printf("Shader builded\n");
}

Shader::~Shader() { glDeleteShader(shader_reference); };
} // namespace WE::Render
