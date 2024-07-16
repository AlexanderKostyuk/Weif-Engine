#ifndef WE_RENDER_OPENGL_RENDERER_2D_H
#define WE_RENDER_OPENGL_RENDERER_2D_H

#include "GL/glcorearb.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "render/opengl/program.h"
#include <vector>

namespace WE::Render::Opengl {

class Renderer2D {

public:
  Renderer2D();
  inline void RenderSprite(const glm::mat4 &transform, const GLuint sprite) {
    RenderSprites({transform}, sprite);
  }
  void RenderSprites(const std::vector<glm::mat4> &transforms,
                     const GLuint sprite);

private:
  Program default_program_;
  GLuint quad_vao_;
  GLuint vbo_;
  GLuint transforms_buffer_;
};

} // namespace WE::Render::Opengl

#endif // !WE_RENDER_OPENGL_RENDERER_2D_H
