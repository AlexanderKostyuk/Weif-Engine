#define _USE_MATH_DEFINES
#include "systems/light_travel_system.h"
#include "glm/gtx/string_cast.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "ECS/components/point_light.h"
#include "application.h"
#include "components/traveling_light.h"
namespace Demo::Systems {

glm::vec3 LightTravelSystem::CalculatePosition(
    const Demo::Components::TravelingLight &traveling_light) {
  glm::vec3 position = traveling_light.start_position;
  position.x += sin(traveling_light.current_time) * 8;
  position.z += sin(traveling_light.current_time * 2) * 4;
  return position;
}

void LightTravelSystem::Update(float delta_time) {

  auto &coordinator = GetApplication().GetCoordinator();
  auto entities = coordinator.GetEntities<WE::ECS::Components::PointLight,
                                          Demo::Components::TravelingLight>();

  for (auto entity : entities) {
    auto &traveling_light =
        coordinator.GetComponent<Demo::Components::TravelingLight>(entity);
    auto &point_light =
        coordinator.GetComponent<WE::ECS::Components::PointLight>(entity);
    traveling_light.current_time += delta_time * speed_ * traveling_light.speed;
    while (traveling_light.current_time >= M_PI * 2.0f)
      traveling_light.current_time =
          std::fmod(traveling_light.current_time, M_PI * 2.0f);
    point_light.position = CalculatePosition(traveling_light);
  }
  ProcessInput(delta_time);
}

void LightTravelSystem::ProcessInput(float delta_time) {
  auto &input_manager = GetApplication().GetInputManager();
  float speed_change = input_manager.GetKeyState(GLFW_KEY_RIGHT) -
                       input_manager.GetKeyState(GLFW_KEY_LEFT);
  speed_ += speed_change * speed_change_speed_ * delta_time;
}

} // namespace Demo::Systems
