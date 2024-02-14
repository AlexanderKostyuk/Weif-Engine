#ifndef WE_RENDER_LIGHTING_POINT_LIGHT_H
#define WE_RENDER_LIGHTING_POINT_LIGHT_H

#include "render/lighting/light_info.h"
#include "render/texture_manager.h"
namespace WE::Render::Lighting {

struct PointLight {
  glm::vec4 position;
  LightInfo light_info;
  glm::vec4 intensity;
  TextureId shadow_map_texture;
};

} // namespace WE::Render::Lighting

#endif // !WE_RENDER_LIGHTING_POINT_LIGHT_H
