#ifndef WE_ECS_COMPONENTS_DIRECTIONAL_LIGHT_H
#define WE_ECS_COMPONENTS_DIRECTIONAL_LIGHT_H

#include "glm/glm.hpp"

namespace WE::ECS::Components {

struct DirectionalLight {
  glm::vec3 direction = glm::vec3(0.0f, -1.0f, 1.0f);
  glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
  glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
  glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
};

} // namespace WE::ECS::Components

#endif // !WE_ECS_COMPONENTS_DIRECTIONAL_LIGHT_H
