#include "systems/camera_control_system.h"

#include "application.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include "input/input_manager.h"

namespace Demo::Systems {

void CameraControlSystem::Update(float delta_time) {
  RotateCamera(delta_time);
  MoveCamera(delta_time);
}

void CameraControlSystem::RotateCamera(float delta_time) {

  auto &input_manager = GetApplication().GetInputManager();
  bool cursor_locked = input_manager.IsCursorLocked();

  if (!input_manager.GetMouseKeyState(GLFW_MOUSE_BUTTON_LEFT)) {
    if (cursor_locked)
      input_manager.UnlockCursor();
    return;
  }

  if (!cursor_locked)
    input_manager.LockCursor();

  glm::vec2 rotation = input_manager.GetMouseMovement();

  if (std::abs(rotation.x) < deadlift_)
    rotation.x = 0;
  if (std::abs(rotation.y) < deadlift_)
    rotation.y = 0;

  rotation *= camera_rotation_speed_ * delta_time;

  glm::quat pitch(glm::vec3(rotation.y, 0.0f, 0.0f));
  glm::quat yaw(glm::vec3(0.0f, rotation.x, 0.0f));

  auto &camera = GetApplication().GetCamera();
  camera.rotation = camera.rotation * yaw;
  camera.rotation = pitch * camera.rotation;
}

void CameraControlSystem::MoveCamera(float delta_time) {
  glm::vec3 camera_movement(0.0f);
  camera_movement.x = GetKey(GLFW_KEY_A) - GetKey(GLFW_KEY_D);
  camera_movement.z = GetKey(GLFW_KEY_W) - GetKey(GLFW_KEY_S);

  auto &camera = GetApplication().GetCamera();

  camera_movement = camera_movement * camera.rotation;
  camera.position += camera_movement * delta_time * 5.0f;
}

} // namespace Demo::Systems
