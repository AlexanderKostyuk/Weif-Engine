#ifndef DEMO_COMPONENTS_TRAVELING_LIGHT_H
#define DEMO_COMPONENTS_TRAVELING_LIGHT_H

#include "glm/glm.hpp"

namespace Demo::Components {
struct TravelingLight {
  float current_time{};
  glm::vec3 start_position{};
  float speed{};
};
} // namespace Demo::Components

#endif // !DEMO_COMPONENTS_TRAVELING_LIGHT_H
