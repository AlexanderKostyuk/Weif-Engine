#include "render/render_system.h"

#include "ECS/components/directional_light.h"
#include "ECS/components/material.h"
#include "ECS/components/mesh_renderer.h"
#include "ECS/components/point_light.h"
#include "ECS/components/sprite_2d.h"
#include "ECS/components/transform.h"
#include "ECS/coordinator.h"
#include "application.h"
#include "glm/ext/matrix_transform.hpp"
#include "render/i_pipeline.h"
#include "render/lighting/point_light.h"
#include "render/texture_manager.h"

namespace WE::Render {

RenderSystem::RenderSystem(WE::Application &application)
    : ISystem(application) {}

void RenderSystem::Update(float delta_time) {

  auto &pipeline = GetApplication().GetPipeline();
  auto &coordinator = GetApplication().GetCoordinator();

  {
    auto directional_lights =
        coordinator.GetEntities<WE::ECS::Components::DirectionalLight>();
    if (directional_lights.empty()) {
      pipeline.SetDirectionalLight();
    } else {
      auto &component =
          coordinator.GetComponent<WE::ECS::Components::DirectionalLight>(
              directional_lights[0]);
      if (component.shadow_map_id == 0)
        component.shadow_map_id =
            pipeline.GetTextureManager().Create2DShadowMap(
                pipeline.GetShadowSize());

      Lighting::DirectionalLight directional_light{
          .direction = glm::vec4(component.direction, 1.0f),
          .light_info = {.ambient = component.ambient,
                         .diffuse = component.diffuse,
                         .specular = component.specular},
          .shadow_map_texture = component.shadow_map_id};

      pipeline.SetDirectionalLight(directional_light);
    }
  }

  {
    auto point_light_entities =
        coordinator.GetEntities<WE::ECS::Components::PointLight>();
    std::vector<Lighting::PointLight> point_lights{};
    point_lights.reserve(point_light_entities.size());
    for (auto entity : point_light_entities) {
      auto &component =
          coordinator.GetComponent<WE::ECS::Components::PointLight>(entity);
      if (component.shadow_map_id == 0)
        component.shadow_map_id =
            pipeline.GetTextureManager().CreateCubeShadowMap(
                pipeline.GetShadowSize());
      point_lights.emplace_back(Lighting::PointLight{
          .position = glm::vec4(component.position, 1.0f),
          .light_info = {.ambient = component.ambient,
                         .diffuse = component.diffuse,
                         .specular = component.specular},
          .intensity = glm::vec4(component.intensity, 0.0f),
          .shadow_map_texture = component.shadow_map_id});
    }
    pipeline.SetPointLights(point_lights);
  }

  {
    auto object_entities =
        coordinator.GetEntities<WE::ECS::Components::Material,
                                WE::ECS::Components::Transform,
                                WE::ECS::Components::MeshRenderer>();
    std::vector<Object> objects{};
    objects.reserve(object_entities.size());
    for (auto entity : object_entities) {
      auto &transform =
          coordinator.GetComponent<WE::ECS::Components::Transform>(entity);
      auto &material =
          coordinator.GetComponent<WE::ECS::Components::Material>(entity);
      auto &mesh_renderer =
          coordinator.GetComponent<WE::ECS::Components::MeshRenderer>(entity);
      glm::mat4 transform_matrix =
          glm::translate(glm::mat4(1.0f), transform.position) *
          glm::mat4(transform.rotation) *
          glm::scale(glm::mat4(1.0f), transform.scale);
      objects.emplace_back(
          Object{.transform = transform_matrix,
                 .diffuse_texture_id = material.diffuse_texture_id,
                 .specular_texture_id = material.specular_texture_id,
                 .mesh_id = mesh_renderer.mesh_id});
    }
    pipeline.SetObjects(std::move(objects));
  }

  {
    auto objects_2d_entities =
        coordinator.GetEntities<WE::ECS::Components::Transform,
                                WE::ECS::Components::Sprite2D>();
    std::unordered_map<TextureId, std::vector<glm::mat4>> packed_objects;
    for (auto entity : objects_2d_entities) {
      auto &transform =
          coordinator.GetComponent<WE::ECS::Components::Transform>(entity);
      auto &sprite_2d =
          coordinator.GetComponent<WE::ECS::Components::Sprite2D>(entity);

      glm::mat4 transform_matrix =
          glm::translate(glm::mat4(1.0f), transform.position) *
          glm::mat4(transform.rotation) *
          glm::scale(glm::mat4(1.0f), transform.scale);

      packed_objects[sprite_2d.sprite_id].push_back(transform_matrix);
    }
    pipeline.SetObjects2D(std::move(packed_objects));
  }

  pipeline.Draw();
}

} // namespace WE::Render
