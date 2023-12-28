#ifndef WE_ECS_COMPONENTS_DIRECTIONAL_LIGHT_H
#define WE_ECS_COMPONENTS_DIRECTIONAL_LIGHT_H

#include "glm/glm.hpp"

namespace WE::ECS::Components {

struct DirectionalLight {
  glm::vec3 direction = glm::vec3(0.0f, -1.0f, 1.0f);
  glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
  glm::vec4 diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
  glm::vec4 specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
};

} // namespace WE::ECS::Components

#endif // !WE_ECS_COMPONENTS_DIRECTIONAL_LIGHT_H
