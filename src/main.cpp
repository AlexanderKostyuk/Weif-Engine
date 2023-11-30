#define GLFW_DLL
#include <glm/fwd.hpp>
#include <vector>

#include "ECS/components/material.h"
#include "ECS/components/mesh_renderer.h"
#include "ECS/components/transform.h"
#include "application.h"
#include "camera_control_system.h"
#include "render/model_manager.h"
#include "render/texture_manager.h"

WE::Application application;
WE::Render::TextureId g_cube_albedo_texture;
WE::Render::TextureId g_plane_albedo_texture;
WE::Render::TextureId g_normal_texture;

WE::Render::MeshId g_flat_cube_mesh;
WE::Render::MeshId g_smooth_cube_mesh;
WE::Render::MeshId g_plane_mesh;

void InitTextures() {
  auto &texture_manager = application.GetTextureManager();

  std::vector<std::vector<std::array<unsigned char, 4>>> albedo_texture{
      std::vector<std::array<unsigned char, 4>>{
          std::array<unsigned char, 4>{0, 128, 0, 255}}};
  g_cube_albedo_texture = texture_manager.LoadTextureFromVector(albedo_texture);

  std::vector<std::vector<std::array<unsigned char, 4>>> plane_albedo_texture{
      std::vector<std::array<unsigned char, 4>>{
          std::array<unsigned char, 4>{192, 192, 192, 255}}};
  g_plane_albedo_texture =
      texture_manager.LoadTextureFromVector(plane_albedo_texture);

  std::vector<std::vector<std::array<unsigned char, 4>>> normal_texture{
      std::vector<std::array<unsigned char, 4>>{
          std::array<unsigned char, 4>{128, 128, 255, 255}}};
  g_normal_texture = texture_manager.LoadTextureFromVector(normal_texture);
}

void InitModels() {
  auto &model_manager = application.GetModelManager();

  std::vector<glm::vec3> cube_vertices{
      glm::vec3(-0.5f, -0.5f, 0.5f),  glm::vec3(-0.5f, 0.5f, 0.5f),
      glm::vec3(0.5f, 0.5f, 0.5f),    glm::vec3(0.5f, -0.5f, 0.5f),
      glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, -0.5f),
      glm::vec3(0.5f, 0.5f, -0.5f),   glm::vec3(0.5f, -0.5f, -0.5f),
  };
  std::vector<glm::vec2> cube_uv{glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                                 glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f),
                                 glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
                                 glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)};
  std::vector<glm::uvec3> cube_indices{// front face
                                       glm::uvec3(0, 1, 2), glm::uvec3(0, 2, 3),

                                       // left face
                                       glm::uvec3(5, 1, 0), glm::uvec3(5, 0, 4),

                                       // rigth face
                                       glm::uvec3(3, 2, 7), glm::uvec3(2, 6, 7),

                                       // bottom face
                                       glm::uvec3(0, 3, 7), glm::uvec3(0, 7, 4),

                                       // top face
                                       glm::uvec3(1, 5, 2), glm::uvec3(2, 5, 6),

                                       // back face
                                       glm::uvec3(7, 6, 5),
                                       glm::uvec3(7, 5, 4)};
  g_flat_cube_mesh =
      model_manager.LoadModel(model_manager.GenerateModelFlatShading(
          cube_vertices, cube_uv, cube_indices));
  g_smooth_cube_mesh =
      model_manager.LoadModel(model_manager.GenerateModelSmoothShading(
          cube_vertices, cube_uv, cube_indices));

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
  g_plane_mesh = model_manager.LoadModel(model_manager.GenerateModelFlatShading(
      plane_vertices, plane_uv, plane_indices));
}

void InitSystems() {
  auto &coordinator = application.GetCoordinator();
  coordinator.RegisterSystem<WE::CameraControlSystem>(&application);
}

void InitEntities() {
  auto &coordinator = application.GetCoordinator();
  auto cube = coordinator.CreateEntity();
  coordinator.AddComponent(cube, WE::ECS::Components::Transform{
                                     .position = glm::vec3(2.0f, 0.0f, -5.0f)});
  coordinator.AddComponent(
      cube, WE::ECS::Components::MeshRenderer{.mesh_id = g_flat_cube_mesh});
  coordinator.AddComponent(cube, WE::ECS::Components::Material{
                                     .albedo_texture_id = g_cube_albedo_texture,
                                     .normal_texture_id = g_normal_texture});

  auto smooth_cube = coordinator.CreateEntity();
  coordinator.AddComponent(smooth_cube,
                           WE::ECS::Components::Transform{
                               .position = glm::vec3(-2.0f, 0.0f, -5.0f)});
  coordinator.AddComponent(smooth_cube, WE::ECS::Components::MeshRenderer{
                                            .mesh_id = g_smooth_cube_mesh});
  coordinator.AddComponent(
      smooth_cube,
      WE::ECS::Components::Material{.albedo_texture_id = g_cube_albedo_texture,
                                    .normal_texture_id = g_normal_texture});

  auto plane = coordinator.CreateEntity();
  coordinator.AddComponent(plane,
                           WE::ECS::Components::Transform{
                               .position = glm::vec3(0.0f, -1.0f, -5.0f),
                               .scale = glm::vec3(10.0f, 10.0f, 1.0f),
                               .rotation = glm::vec3(-1.556f, 0.0f, 0.0f)});
  coordinator.AddComponent(
      plane, WE::ECS::Components::MeshRenderer{.mesh_id = g_plane_mesh});
  coordinator.AddComponent(
      plane,
      WE::ECS::Components::Material{.albedo_texture_id = g_plane_albedo_texture,
                                    .normal_texture_id = g_normal_texture});
}

int main() {
  InitTextures();
  InitModels();
  InitSystems();
  InitEntities();

  application.Start();
}
