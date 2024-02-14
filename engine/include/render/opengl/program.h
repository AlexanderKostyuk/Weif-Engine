#ifndef WE_RENDER_OPENGL_PROGRAM_H
#define WE_RENDER_OPENGL_PROGRAM_H

#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#include <glm/glm.hpp>
#include <string>
namespace WE::Render::Opengl {

class Program {

public:
  Program(Program &&other) = default;
  Program &operator=(Program &&other) = default;

  Program(const Program &) = delete;
  Program &operator=(const Program &) = delete;

  Program() = default;
  Program(const char *vertex_shader_path, const char *fragment_shader_path);
  Program(std::string &vertex_shader_path, std::string &fragment_shader_path);
  Program(const char *vertex_shader_path, const char *fragment_shader_path,
          const char *geometry_shader_path);
  Program(std::string &vertex_shader_path, std::string &fragment_shader_path,
          std::string &geometry_shader_path);

  inline void UseProgram() { glUseProgram(gl_program_); }
  inline void FreeProgram() { glUseProgram(0); }

private:
  GLuint gl_program_;
};

} // namespace WE::Render::Opengl

#endif // !WE_RENDER_OPENGL_PROGRAM_H
