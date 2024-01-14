#ifndef DEMO_SYSTEMS_LIGHT_TRAVEL_SYSTEM_H
#define DEMO_SYSTEMS_LIGHT_TRAVEL_SYSTEM_H

#include "ECS/i_system.h"
#include "components/traveling_light.h"
#include "glm/glm.hpp"
namespace Demo::Systems {

class LightTravelSystem : public WE::ECS::ISystem {

private:
  glm::vec3
  CalculatePosition(const Demo::Components::TravelingLight &traveling_light);

public:
  LightTravelSystem(WE::Application &application)
      : WE::ECS::ISystem(application) {}
  void Update(float delta_time) override;
  void ProcessInput(float delta_time);

  inline float GetSpeed() { return speed_; }
  inline void SetSpeed(float speed) { speed_ = speed; }

private:
  float speed_ = 0.5f;
  float speed_change_speed_ = 0.10f;
};

} // namespace Demo::Systems

#endif // !DEMO_SYSTEMS_LIGHT_TRAVEL_SYSTEM_H
