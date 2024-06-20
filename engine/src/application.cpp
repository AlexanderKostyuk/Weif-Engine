#include "application.h"
#include "ECS/components/directional_light.h"
#include "ECS/components/point_light.h"
#include "ECS/components/sprite_2d.h"
#include "application_window.h"
#include "render/i_pipeline.h"
#include <chrono>

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
#include "ECS/i_system.h"
#include "input/input_manager.h"
#include "render/render_system.h"

namespace WE {

Application::Application(Render::IPipeline &pipeline, int window_width,
                         int window_height)
    : input_manager_(*this), pipeline_(pipeline),
      application_window_(*this, window_width, window_height) {
  Init();
}

void Application::Start() {
  while (!application_window_.WindowShouldClose())
    Loop();
  Cleanup();
}

void Application::Init() {
  InitWindow();
  InitComponents();
  InitSystems();
}

void Application::InitWindow() {
  printf("Initializing Window...\n");
  pipeline_.SetApplication(this);
  pipeline_.Init();
  printf("Window intialized\n");
}

void Application::InitComponents() {
  printf("Initializing components...\n");
  coordinator_.RegisterComponent<ECS::Components::Transform>();
  coordinator_.RegisterComponent<ECS::Components::MeshRenderer>();
  coordinator_.RegisterComponent<ECS::Components::Material>();
  coordinator_.RegisterComponent<ECS::Components::DirectionalLight>();
  coordinator_.RegisterComponent<ECS::Components::PointLight>();
  coordinator_.RegisterComponent<ECS::Components::Sprite2D>();
  printf("Components intialized\n");
}

void Application::InitSystems() {
  printf("Initializing systems...\n");
  coordinator_.RegisterSystem<Render::RenderSystem>(*this);
  printf("Systems initialized\n");
}

void Application::Loop() {
  auto start_time = std::chrono::high_resolution_clock::now();
  coordinator_.UpdateSystems(delta_time_);
  delta_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now() - start_time)
                    .count() /
                1000.0f;
  input_manager_.CleanMovement();
  application_window_.PollEvents();
}

void Application::Cleanup() {}

} // namespace WE
