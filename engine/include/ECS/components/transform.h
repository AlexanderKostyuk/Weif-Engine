#ifndef WE_ECS_COMPONENTS_TRANSFORM_H
#define WE_ECS_COMPONENTS_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace WE::ECS::Components {

struct Transform {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

} // namespace WE::ECS::Components

#endif // !WE_ECS_COMPONENTS_TRANSFORM_H
