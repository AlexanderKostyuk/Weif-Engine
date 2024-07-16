#ifndef WE_RENDER_OPENGL_RENDERER_3D_H
#define WE_RENDER_OPENGL_RENDERER_3D_H

#include "GL/glcorearb.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "render/opengl/program.h"
#include <vector>

namespace WE::Render::Opengl {

struct ModelInfo {
  GLuint vao;
  GLuint transform_buffer;
  GLuint elements_amount;

  GLuint diffuse_texture;
  GLuint specular_texture;
};

class Renderer3D {

public:
  Renderer3D();
  inline void RenderObject(const glm::mat4 &transform, const ModelInfo &model) {
    RenderObjects({transform}, model);
  }
  void RenderObjects(const std::vector<glm::mat4> &transforms,
                     const ModelInfo &model);

private:
  Program default_program_;
};

} // namespace WE::Render::Opengl

#endif // !WE_RENDER_OPENGL_RENDERER_3D_H
