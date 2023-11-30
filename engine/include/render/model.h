#ifndef WE_RENDER_MODEL_H
#define WE_RENDER_MODEL_H

#include <GL/glcorearb.h>
#include <glm/fwd.hpp>
#include <vector>

namespace WE::Render {

struct Model {
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uv;
  std::vector<glm::uvec3> indices;
};

} // namespace WE::Render

#endif // !WE_RENDER_MODEL_H
