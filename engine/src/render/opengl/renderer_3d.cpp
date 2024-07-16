#include "render/opengl/renderer_3d.h"

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "glm/gtc/type_ptr.hpp"

namespace WE::Render::Opengl {

Renderer3D::Renderer3D() {}

void Renderer3D::RenderObjects(const std::vector<glm::mat4> &transforms,
                               const ModelInfo &model) {

  glBindBuffer(GL_ARRAY_BUFFER, model.transform_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * transforms.size(),
               glm::value_ptr(transforms[0]), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, model.diffuse_texture);
  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, model.specular_texture);

  glBindVertexArray(model.vao);
  glDrawElementsInstanced(GL_TRIANGLES, model.elements_amount, GL_UNSIGNED_INT,
                          (void *)0, transforms.size());
  glBindVertexArray(0);
}

} // namespace WE::Render::Opengl
