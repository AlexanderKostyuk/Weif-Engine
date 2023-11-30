#ifndef WE_INPUT_INPUT_MANAGER_H
#define WE_INPUT_INPUT_MANAGER_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

namespace WE {
class Application;
}

namespace WE::Input {

using Key = int;
using KeyState = float;

class InputManager {
public:
  inline void SetKeyState(Key key, KeyState key_state) {
    key_states_[key] = key_state;
  }
  inline KeyState GetKeyState(Key key) { return key_states_[key]; }

  inline void SetMouseKeyState(Key key, KeyState key_state) {
    mouse_states_[key] = key_state;
  }

  inline KeyState GetMouseKeyState(Key key) { return mouse_states_[key]; }

  void SetMousePosition(glm::vec2 &mouse_position);
  void LockCursor();
  void UnlockCursor();
  void CleanMovement();

  inline glm::vec2 GetMousePosition() { return mouse_position_; }
  inline glm::vec2 GetMouseMovement() { return mouse_movement_; }

  inline bool IsCursorLocked() { return cursor_locked_; }

  InputManager(InputManager &&other) = default;
  InputManager &operator=(InputManager &&other) = delete;

  InputManager(const InputManager &) = delete;
  InputManager &operator=(const InputManager &) = delete;

  InputManager(Application &application) : application_(application) {}

private:
  std::unordered_map<Key, KeyState> key_states_{};
  std::unordered_map<Key, KeyState> mouse_states_{};
  glm::vec2 mouse_position_{};
  glm::vec2 real_mouse_position_{};
  glm::vec2 mouse_movement_{};

  bool cursor_locked_ = false;
  Application &application_;
};

} // namespace WE::Input

#endif // !WE_INPUT_INPUT_MANAGER_H
