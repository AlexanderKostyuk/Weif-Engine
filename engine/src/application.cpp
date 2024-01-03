#include "application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ECS/components/material.h"
#include "ECS/components/mesh_renderer.h"
#include "ECS/components/transform.h"
#include "ECS/entity_manager.h"
#include "ECS/i_system.h"
#include "input/input_manager.h"
#include "render/render_system.h"
namespace {

void glfwErrorCallback(int error, const char *message) {
  printf("GLFW Error: %s\n", message);
}
void glfwWindowSizeCallback(GLFWwindow *window, int width, int height) {
  WE::Application *application =
      (WE::Application *)glfwGetWindowUserPointer(window);
  application->SetWindowSize(width, height);
}
void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action,
                     int mods) {
  WE::Application *application =
      (WE::Application *)glfwGetWindowUserPointer(window);
  WE::Input::KeyState key_state = 1.0f;
  if (action == GLFW_RELEASE) {
    key_state = 0.0f;
  }
  auto &input_manager = application->GetInputManager();
  input_manager.SetKeyState(key, key_state);
}
void glfwCursorPositionCallback(GLFWwindow *window, double x_position,
                                double y_position) {
  WE::Application *application =
      (WE::Application *)glfwGetWindowUserPointer(window);
  glm::vec2 mouse_position = glm::vec2(x_position, y_position);
  auto &input_manager = application->GetInputManager();
  input_manager.SetMousePosition(mouse_position);
}
void glfwMouseButtonCallback(GLFWwindow *window, int button, int action,
                             int mods) {
  WE::Application *application =
      (WE::Application *)glfwGetWindowUserPointer(window);
  WE::Input::KeyState key_state = 1.0f;
  if (action == GLFW_RELEASE) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    key_state = 0.0f;
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
  auto &input_manager = application->GetInputManager();
  input_manager.SetMouseKeyState(button, key_state);
}

} // namespace

namespace WE {

Application::Application()
    : window_width_(800), window_height_(600), input_manager_(*this) {
  Init();
}

void Application::Start() {
  while (!glfwWindowShouldClose(window_))
    Loop();
  Cleanup();
}

void Application::Init() {
  InitWindow();
  InitComponents();
  InitSystems();
}

void Application::InitWindow() {

  glfwInit();
  glfwSetErrorCallback(glfwErrorCallback);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window_ = glfwCreateWindow(window_width_, window_height_,
                             "Weif Engine -- Demo", nullptr, nullptr);
  glfwSetWindowUserPointer(window_, this);

  if (window_ == nullptr) {
    fprintf(stderr, "Failed to create window\n");
  }

  glfwMakeContextCurrent(window_);
  glfwSetWindowSizeCallback(window_, glfwWindowSizeCallback);
  glfwSetKeyCallback(window_, glfwKeyCallback);
  glfwSetCursorPosCallback(window_, glfwCursorPositionCallback);
  glfwSetMouseButtonCallback(window_, glfwMouseButtonCallback);
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  gl3wInit();
}

void Application::InitComponents() {
  coordinator_.RegisterComponent<ECS::Components::Transform>();
  coordinator_.RegisterComponent<ECS::Components::MeshRenderer>();
  coordinator_.RegisterComponent<ECS::Components::Material>();
}

void Application::InitSystems() {
  render_system_ = coordinator_.RegisterSystem<Render::RenderSystem>(this);
  render_system_->SetViewportSize(window_width_, window_height_);
}

void Application::Loop() {
  GLfloat start_time = glfwGetTime();
  coordinator_.UpdateSystems(delta_time_);
  delta_time_ = glfwGetTime() - start_time;
  input_manager_.CleanMovement();
  glfwPollEvents();
}

void Application::Cleanup() {
  glfwDestroyWindow(window_);
  glfwTerminate();
}

void Application::SetWindowSize(GLuint width, GLuint height) {
  window_width_ = width;
  window_height_ = height;

  if (render_system_ == nullptr)
    return;
  render_system_->SetViewportSize(width, height);
}

} // namespace WE
