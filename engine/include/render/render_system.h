
#ifndef WE_RENDER_RENDER_SYSTEM_H
#define WE_RENDER_RENDER_SYSTEM_H

#include <GL/glcorearb.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "ECS/components/directional_light.h"
#include "ECS/components/material.h"
#include "ECS/components/transform.h"
#include "ECS/entity_manager.h"
#include "ECS/i_system.h"
#include "render/model_manager.h"
#include "render/program.h"

namespace WE::Render {

class RenderSystem : public ECS::ISystem {
private:
  void GenerateProgram();
  void GenerateGlobalUBO();
  void UpdatePerspectiveMatrix();
  void Draw();
  void DrawMesh(MeshId mesh_id);
  void RenderShadowMaps(const std::vector<WE::ECS::Entity> &light_entities,
                        const std::vector<WE::ECS::Entity> &object_entities);
  void BindDirectionalLight();
  void BindPointLights(std::vector<WE::ECS::Entity> &lights);

  void BindUniforms(ECS::Components::Transform &transform,
                    glm::mat4 &camera_matrix);
  void BindTextures(ECS::Components::Material &material);

  glm::mat4 GetCameraMatrix();

public:
  RenderSystem(WE::Application &application);
  void Update(float delta_time) override;

  void SetViewportSize(GLuint width, GLuint height);
  void SetFov(GLuint fov);
  void SetZ(GLfloat z_near, GLfloat z_far);

private:
  GLuint viewport_width_ = 800;
  GLuint viewport_height_ = 600;
  GLfloat fov_ = 45.0f;
  GLfloat z_near_ = 0.1f;
  GLfloat z_far_ = 100.0f;

  Program base_program_;
  Program shadow_map_program_;

  GLuint shadow_map_framebuffer_;

  GLuint projection_global_UBO_;
  GLuint directional_light_global_UBO_;
  GLuint point_light_global_UBO_;
};
} // namespace WE::Render

#endif // !WE_RENDER_SYSTEM_H
