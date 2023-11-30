#ifndef WE_CAMERA_CONTROL_SYSTEM_H
#define WE_CAMERA_CONTROL_SYSTEM_H

#include "ECS/i_system.h"

#include <glm/ext/scalar_constants.hpp>
#include <glm/glm.hpp>

#include "application.h"
#include "input/input_manager.h"
namespace WE {

class CameraControlSystem : public WE::ECS::ISystem {
private:
  inline Input::KeyState GetKey(Input::Key key) {
    return GetApplication()->GetInputManager().GetKeyState(key);
  }

  void RotateCamera(float delta_time);
  void MoveCamera(float delta_time);

public:
  void Update(float delta_time) override;

private:
  float camera_rotation_speed_ = glm::pi<float>() / 4.0f;
  float camera_speed_ = 5.0f;
  bool camera_rotation_started_ = false;
  float deadlift_ = 2.0f;
};

} // namespace WE
#endif // !WE_CAMERA_CONTROL_SYSTEM_H
