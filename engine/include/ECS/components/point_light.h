#ifndef WE_ECS_COMPONENTS_POINT_LIGHT_H
#define WE_ECS_COMPONENTS_POINT_LIGHT_H

#include "glm/glm.hpp"

#include "render/texture_manager.h"

namespace WE::ECS::Components {

struct PointLight {

  glm::vec3 position = glm::vec3(0.0f);
  glm::vec4 ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
  glm::vec4 diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
  glm::vec4 specular = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
  glm::vec3 intensity = glm::vec3(1.0f, 0.045f, 0.0075f);
  Render::TextureId shadow_map_id = 0;
};
} // namespace WE::ECS::Components

#endif // !WE_ECS_COMPONENTS_POINT_LIGHT_H
