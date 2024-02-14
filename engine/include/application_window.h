#ifndef WE_APPLICATION_WINDOW_H
#define WE_APPLICATION_WINDOW_H

#include "glm/ext/vector_int2.hpp"
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

namespace WE {
class Application;
}

namespace WE {

class ApplicationWindow {

public:
  ApplicationWindow() = delete;
  ApplicationWindow(WE::Application &application, int width, int height);
  ~ApplicationWindow();

  inline void PollEvents() { glfwPollEvents(); }
  inline void SwapBuffers() { glfwSwapBuffers(window_); }
  inline bool WindowShouldClose() { return glfwWindowShouldClose(window_); }
  inline void LockCursor() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    cursor_locked_ = true;
  }
  inline void UnlockCursor() {
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    cursor_locked_ = false;
  }

  inline WE::Application &GetApplication() { return application_; }
  inline bool IsCursorLocked() const { return cursor_locked_; }

  glm::ivec2 GetWindowSize() const;
  void SetWindowSize(int width, int height);

private:
  WE::Application &application_;
  GLFWwindow *window_;

  bool cursor_locked_;
};

} // namespace WE

#endif // !WE_RENDER_APPLICATION_WINDOW_H
