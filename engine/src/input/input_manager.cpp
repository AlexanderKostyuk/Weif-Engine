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
  cursor_locked_ = true;
  glfwSetInputMode(application_.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  mouse_movement_ = glm::vec2(0.0f);
}

void InputManager::UnlockCursor() {
  cursor_locked_ = false;
  glfwSetInputMode(application_.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void InputManager::CleanMovement() { mouse_movement_ = glm::vec2(0.0f); }

} // namespace WE::Input
