#include "ECS/components/sprite_2d.h"
#include "render/texture_manager.h"
#define GLFW_DLL
#define _USE_MATH_DEFINES
#include <glm/fwd.hpp>
#include <math.h>
#include <vector>

#include "ECS/components/directional_light.h"
#include "ECS/components/material.h"
#include "ECS/components/mesh_renderer.h"
#include "ECS/components/point_light.h"
#include "ECS/components/transform.h"
#include "application.h"
#include "components/traveling_light.h"
#include "render/model_manager.h"
#include "render/opengl/opengl_pipeline.h"
#include "systems/camera_control_system.h"
#include "systems/light_travel_system.h"

WE::Render::Opengl::OpenglPipeline pipeline;
WE::Render::MeshId kCubeMesh;
WE::Render::MeshId kSphereMesh;
WE::Render::MeshId kPlaneMesh;

WE::Render::TextureId sprite;

void InitTextures() {
  auto &texture_manager = pipeline.GetTextureManager();
  sprite = texture_manager.LoadTexture(
      WE::Render::TextureRGBA{{{255, 0, 0, 255}, {0, 255, 0, 255}},
                              {{0, 0, 255, 255}, {255, 0, 0, 255}}});
}

void InitModels() {
  auto &model_manager = pipeline.GetModelManager();

  kCubeMesh = model_manager.LoadModelsFromFile("./assets/models/cube.obj")[0];
  kSphereMesh =
      model_manager.LoadModelsFromFile("./assets/models/sphere.obj")[0];

  std::vector<glm::vec3> plane_vertices{
      glm::vec3(-0.5f, -0.5f, -0.5f),
      glm::vec3(-0.5f, 0.5f, -0.5f),
      glm::vec3(0.5f, 0.5f, -0.5f),
      glm::vec3(0.5f, -0.5f, -0.5f),
  };

  std::vector<glm::vec2> plane_uv{glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                                  glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)};
  std::vector<glm::uvec3> plane_indices{glm::uvec3(0, 1, 2),
                                        glm::uvec3(0, 2, 3)};
  kPlaneMesh = model_manager.LoadModel(model_manager.GenerateModelFlatShading(
      plane_vertices, plane_uv, plane_indices));
}

void InitSystems(WE::Application &application) {
  printf("Demo systems initialization...\n");
  auto &coordinator = application.GetCoordinator();
  coordinator.RegisterSystem<Demo::Systems::CameraControlSystem>(application);
  coordinator.RegisterSystem<Demo::Systems::LightTravelSystem>(application);
  printf("Demo systems initialized.\n");
}

void InitComponents(WE::Application &application) {
  printf("Demo components initialization...\n");
  auto &coordinator = application.GetCoordinator();
  coordinator.RegisterComponent<Demo::Components::TravelingLight>();
  printf("Demo components initialized.\n");
}

void InitEntities(WE::Application &application) {
  printf("Demo entities initialization...\n");
  auto &coordinator = application.GetCoordinator();
  auto cube = coordinator.CreateEntity();
  coordinator.AddComponent(cube, WE::ECS::Components::Transform{
                                     .position = glm::vec3(3.0f, 0.0f, -5.0f)});
  coordinator.AddComponent(
      cube, WE::ECS::Components::MeshRenderer{.mesh_id = kCubeMesh});
  coordinator.AddComponent(cube, WE::ECS::Components::Material{});

  auto smooth_cube = coordinator.CreateEntity();
  coordinator.AddComponent(smooth_cube,
                           WE::ECS::Components::Transform{
                               .position = glm::vec3(-3.0f, 0.0f, -5.0f)});
  coordinator.AddComponent(
      smooth_cube, WE::ECS::Components::MeshRenderer{.mesh_id = kSphereMesh});
  coordinator.AddComponent(smooth_cube, WE::ECS::Components::Material{});

  auto plane = coordinator.CreateEntity();
  coordinator.AddComponent(plane,
                           WE::ECS::Components::Transform{
                               .position = glm::vec3(0.0f, -1.0f, -5.0f),
                               .scale = glm::vec3(20.0f, 20.0f, 1.0f),
                               .rotation = glm::vec3(-1.556f, 0.0f, 0.0f)});
  coordinator.AddComponent(
      plane, WE::ECS::Components::MeshRenderer{.mesh_id = kPlaneMesh});
  coordinator.AddComponent(plane, WE::ECS::Components::Material{});

  auto dir_light = coordinator.CreateEntity();
  coordinator.AddComponent(dir_light,
                           WE::ECS::Components::DirectionalLight{
                               .direction = glm::vec3(0.0f, -1.0f, 1.0f),
                               .ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),
                               .diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
                               .specular = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)});
  auto point_light_1 = coordinator.CreateEntity();
  coordinator.AddComponent(point_light_1,
                           WE::ECS::Components::PointLight{
                               .ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),
                               .diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
                               .specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)});
  coordinator.AddComponent(
      point_light_1,
      Demo::Components::TravelingLight{
          .start_position = glm::vec3(0.0f, 0.0f, -5.0f), .speed = 0.0f});
  auto point_light_2 = coordinator.CreateEntity();
  coordinator.AddComponent(point_light_2,
                           WE::ECS::Components::PointLight{
                               .ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),
                               .diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
                               .specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)});
  coordinator.AddComponent(point_light_2,
                           Demo::Components::TravelingLight{
                               .current_time = 0.5f * M_PI,
                               .start_position = glm::vec3(0.0f, 0.0f, -5.0f),
                               .speed = 0.5f});
  auto point_light_3 = coordinator.CreateEntity();
  coordinator.AddComponent(point_light_3,
                           WE::ECS::Components::PointLight{
                               .ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f),
                               .diffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
                               .specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f)});
  coordinator.AddComponent(point_light_3,
                           Demo::Components::TravelingLight{
                               .current_time = 1.5f * M_PI,
                               .start_position = glm::vec3(0.0f, 0.0f, -5.0f),
                               .speed = 1.5f});

  auto sprite_2d = coordinator.CreateEntity();
  coordinator.AddComponent(sprite_2d, WE::ECS::Components::Transform{});
  coordinator.AddComponent(sprite_2d, WE::ECS::Components::Sprite2D{sprite});

  printf("Demo entities initialized\n");
}

int main() {
  WE::Application application(pipeline);
  InitTextures();
  InitModels();
  InitComponents(application);
  InitSystems(application);
  InitEntities(application);

  application.Start();
}
