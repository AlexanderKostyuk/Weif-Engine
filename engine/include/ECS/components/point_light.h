#ifndef WE_ECS_COMPONENTS_POINT_LIGHT_H
#define WE_ECS_COMPONENTS_POINT_LIGHT_H

#include "glm/glm.hpp"

namespace WE::ECS::Components {

struct PointLight {

  glm::vec3 position = glm::vec3(0.0f);

  glm::vec3 ambient = glm::vec3(0.2f);
  glm::vec3 diffuse = glm::vec3(0.4f);
  glm::vec3 specular = glm::vec3(0.4f);
  glm::vec3 intensity = glm::vec3(1.0f, 0.045f, 0.0075f);
};
} // namespace WE::ECS::Components

#endif // !WE_ECS_COMPONENTS_POINT_LIGHT_H
