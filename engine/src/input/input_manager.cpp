#include "input/input_manager.h"

#include "application.h"
#include <glm/gtx/string_cast.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace WE::Input {

void InputManager::SetMousePosition(glm::vec2 &new_mouse_position) {
  mouse_movement_ = new_mouse_position - mouse_position_;
  mouse_position_ = new_mouse_position;
}

void InputManager::LockCursor() {
  application_.GetWindow().LockCursor();
  mouse_movement_ = glm::vec2(0.0f);
}

void InputManager::UnlockCursor() { application_.GetWindow().UnlockCursor(); }
bool InputManager::IsCursorLocked() {
  return application_.GetWindow().IsCursorLocked();
}

void InputManager::CleanMovement() { mouse_movement_ = glm::vec2(0.0f); }

} // namespace WE::Input
