#ifndef WE_RENDER_LIGHTING_LIGHT_INFO_H
#define WE_RENDER_LIGHTING_LIGHT_INFO_H

#include "glm/glm.hpp"

namespace WE::Render::Lighting {

struct LightInfo {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
};

} // namespace WE::Render::Lighting

#endif // !WE_RENDER_LIGHT_INFO_H
