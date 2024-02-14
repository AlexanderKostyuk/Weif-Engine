#ifndef WE_RENDER_LIGHTING_DIRECTIONAL_LIGHT_H
#define WE_RENDER_LIGHTING_DIRECTIONAL_LIGHT_H
#include "ECS/components/directional_light.h"
#include "glm/glm.hpp"
#include "render/lighting/light_info.h"
#include "render/texture_manager.h"
namespace WE::Render::Lighting {
struct DirectionalLight {
  glm::vec4 direction;
  LightInfo light_info;
  TextureId shadow_map_texture;
};
} // namespace WE::Render::Lighting
#endif // !WE_RENDER_DIRECTIONAL_LIGHT_H
