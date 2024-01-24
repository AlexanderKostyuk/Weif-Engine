#ifndef WE_RENDER_LIGHTING_LIGHT_INFO
#define WE_RENDER_LIGHTING_LIGHT_INFO

#include "glm/glm.hpp"

namespace WE::Render {
struct LightInfo {
  glm::vec4 ambient{};
  glm::vec4 diffuse{};
  glm::vec4 specular{};
};

} // namespace WE::Render

#endif // !WE_RENDER_LIGHTING_LIGHT_INFO
