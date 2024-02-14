#include "application_window.h"

#include "GLFW/glfw3.h"
#include "application.h"
#include "glm/ext/vector_int2.hpp"

namespace {

void ErrorCallback(int error, const char *message) {
  printf("GLFW Error: %s\n", message);
}
void WindowResizeCallback(GLFWwindow *window, int width, int height) {
  auto *application_window =
      (WE::ApplicationWindow *)glfwGetWindowUserPointer(window);
  application_window->GetApplication().WindowResizeCallback(width, height);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  auto *application_window =
      (WE::ApplicationWindow *)glfwGetWindowUserPointer(window);

  WE::Input::KeyState key_state = 1.0f;
  if (action == GLFW_RELEASE) {
    key_state = 0.0f;
  }

  application_window->GetApplication().KeyCallback(key, key_state);
}

void CursorPositionCallback(GLFWwindow *window, double x_position,
                            double y_position) {
  auto *application_window =
      (WE::ApplicationWindow *)glfwGetWindowUserPointer(window);

  glm::vec2 mouse_position = glm::vec2(x_position, y_position);

  application_window->GetApplication().CursorPositionCallback(mouse_position);
}

void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  auto *application_window =
      (WE::ApplicationWindow *)glfwGetWindowUserPointer(window);
  WE::Input::KeyState key_state = 1.0f;

  if (action == GLFW_RELEASE) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    key_state = 0.0f;
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  application_window->GetApplication().MouseButtonCallback(button, key_state);
}

} // namespace

namespace WE {

ApplicationWindow::ApplicationWindow(WE::Application &application, int width,
                                     int height)
    : application_(application) {

  glfwInit();
  glfwSetErrorCallback(ErrorCallback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  window_ =
      glfwCreateWindow(width, height, "Weif Engine -- Demo", nullptr, nullptr);
  glfwSetWindowUserPointer(window_, this);
  if (window_ == nullptr) {
    fprintf(stderr, "Failed to create window\n");
  }

  glfwMakeContextCurrent(window_);
  glfwSetWindowSizeCallback(window_, WindowResizeCallback);
  glfwSetKeyCallback(window_, KeyCallback);
  glfwSetCursorPosCallback(window_, CursorPositionCallback);
  glfwSetMouseButtonCallback(window_, MouseButtonCallback);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

ApplicationWindow::~ApplicationWindow() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

glm::ivec2 ApplicationWindow::GetWindowSize() const {
  int width, height;
  glfwGetWindowSize(window_, &width, &height);
  return glm::ivec2(width, height);
}

void ApplicationWindow::SetWindowSize(int width, int height) {

  auto window_size = GetWindowSize();
  if (width == window_size.x && height == window_size.y)
    return;
  glfwSetWindowSize(window_, width, height);
}

} // namespace WE
