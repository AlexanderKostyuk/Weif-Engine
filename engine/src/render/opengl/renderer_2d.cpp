#include "render/opengl/renderer_2d.h"
#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace WE::Render::Opengl {

Renderer2D::Renderer2D() {

  default_program_ =
      Program("./engine/shaders/2d.vert", "./engine/shaders/2d.frag");

  glGenVertexArrays(1, &quad_vao_);
  glGenBuffers(1, &vbo_);
  glGenBuffers(1, &transforms_buffer_);

  float quad_vertices[10]{0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f};

  glBindVertexArray(quad_vao_);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);

  // Bind vertices coordinates to 0 attrib array
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, quad_vertices,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void *)0);

  // Bind transform buffer to attrib arrays (max pointer size 4, for matrix bind
  // we use 4 pointers with size 4)
  glBindBuffer(GL_ARRAY_BUFFER, transforms_buffer_);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(1 * sizeof(glm::vec4)));
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(2 * sizeof(glm::vec4)));
  glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                        (void *)(3 * sizeof(glm::vec4)));
  // and add division for instancing(stride only on instance change)
  glVertexAttribDivisor(1, 1);
  glVertexAttribDivisor(2, 1);
  glVertexAttribDivisor(3, 1);
  glVertexAttribDivisor(4, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void Renderer2D::RenderSprites(const std::vector<glm::mat4> &transforms,
                               const GLuint sprite) {

  default_program_.UseProgram();

  // Buffer sprite position
  glBindBuffer(GL_ARRAY_BUFFER, transforms_buffer_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * transforms.size(),
               glm::value_ptr(transforms[0]), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Bind sprite texture
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, sprite);

  glBindVertexArray(quad_vao_);
  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, transforms.size());

  glBindVertexArray(0);
  default_program_.FreeProgram();
}

} // namespace WE::Render::Opengl
