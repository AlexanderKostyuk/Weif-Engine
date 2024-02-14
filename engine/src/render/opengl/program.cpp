#include <render/opengl/program.h>

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "render/shader.h"

namespace WE::Render::Opengl {

Program::Program(const char *vertex_shader_path,
                 const char *fragment_shader_path) {

  gl_program_ = glCreateProgram();

  Shader vertex_shader = Shader(vertex_shader_path, GL_VERTEX_SHADER);
  Shader fragment_shader = Shader(fragment_shader_path, GL_FRAGMENT_SHADER);

  glAttachShader(gl_program_, vertex_shader.GetShaderRef());
  glAttachShader(gl_program_, fragment_shader.GetShaderRef());
  glLinkProgram(gl_program_);
  glDetachShader(gl_program_, vertex_shader.GetShaderRef());
  glDetachShader(gl_program_, fragment_shader.GetShaderRef());
}

Program::Program(const char *vertex_shader_path,
                 const char *fragment_shader_path,
                 const char *geometry_shader_path) {

  gl_program_ = glCreateProgram();

  Shader vertex_shader = Shader(vertex_shader_path, GL_VERTEX_SHADER);
  Shader fragment_shader = Shader(fragment_shader_path, GL_FRAGMENT_SHADER);
  Shader geometry_shader = Shader(geometry_shader_path, GL_GEOMETRY_SHADER);

  glAttachShader(gl_program_, vertex_shader.GetShaderRef());
  glAttachShader(gl_program_, fragment_shader.GetShaderRef());
  glAttachShader(gl_program_, geometry_shader.GetShaderRef());
  glLinkProgram(gl_program_);
  glDetachShader(gl_program_, vertex_shader.GetShaderRef());
  glDetachShader(gl_program_, fragment_shader.GetShaderRef());
  glDetachShader(gl_program_, geometry_shader.GetShaderRef());
}

Program::Program(std::string &vertex_shader_path,
                 std::string &fragment_shader_path)
    : Program::Program(vertex_shader_path.c_str(),
                       fragment_shader_path.c_str()) {}
Program::Program(std::string &vertex_shader_path,
                 std::string &fragment_shader_path,
                 std::string &geometry_shader_path)
    : Program::Program(vertex_shader_path.c_str(), fragment_shader_path.c_str(),
                       geometry_shader_path.c_str()) {}

} // namespace WE::Render::Opengl
