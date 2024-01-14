#include "render/render_system.h"

#include "ECS/entity_manager.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "stb_image.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <vector>
#include <winuser.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#include <GL/gl3w.h>
#include <GL/glcorearb.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/matrix.hpp>

#include "ECS/components/directional_light.h"
#include "ECS/components/material.h"
#include "ECS/components/mesh_renderer.h"
#include "ECS/components/point_light.h"
#include "ECS/components/transform.h"
#include "ECS/coordinator.h"
#include "application.h"
#include "render/program.h"
#include "render/texture_manager.h"

namespace {
const GLuint kProjectionUniformBlockIndex = 0;
const GLuint kDirectionalLightUniformBlockIndex = 1;
const GLuint kPointLightsUniformBlockIndex = 2;

const GLuint kModelCameraUniformLocation = 1;
const GLuint kModelCameraNormalUniformLocation = 2;

const GLuint kModelWorldSpaceUniformLocation = 1;
const GLuint kShadowMatricesUniformLocation = 2;
const GLuint kLightPositionUniformLocation = 8;
const GLuint kFarPlaneUniformLocation = 9;

const GLuint kDirectionalLightDirection = 128;
const GLuint kDirectionalLightAmbient = 129;
const GLuint kDirectionalLightDiffuse = 130;
const GLuint kDirectionalLightSpecular = 131;
const GLuint kDirectionalLightExists = 132;
const GLuint kShininess = 133;
const GLuint kAmountOfPointLights = 134;
const GLuint kCameraToWorldRotationMatrixUniformLocation = 135;

uint16_t kShadowMapWidth = 8192, kShadowMapHeight = 8192;

} // namespace

namespace WE::Render {

RenderSystem::RenderSystem(WE::Application &application)
    : ISystem(application) {

  glEnable(GL_CULL_FACE);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glCullFace(GL_FRONT_FACE);
  glFrontFace(GL_CW);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  stbi_set_flip_vertically_on_load(true);

  glGenFramebuffers(1, &shadow_map_framebuffer_);

  GenerateGlobalUBO();
  GenerateProgram();
}

void RenderSystem::GenerateProgram() {
  printf("Generating Program...\n");
  base_program_ =
      Program("./engine/shaders/shader.vert", "./engine/shaders/gaussian.frag");
  shadow_map_program_ = Program("./engine/shaders/shadow_mapping.vert",
                                "./engine/shaders/shadow_mapping.frag",
                                "./engine/shaders/shadow_mapping.geom");
  printf("Program Generated\n");
}

void RenderSystem::GenerateGlobalUBO() {
  printf("Generating Global UBO...\n");

  // Projection Matrix UBO
  glGenBuffers(1, &projection_global_UBO_);
  glBindBuffer(GL_UNIFORM_BUFFER, projection_global_UBO_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STREAM_DRAW);

  glm::mat4 perspective_matrix = glm::perspective(
      fov_, (float)viewport_width_ / (float)viewport_height_, z_near_, z_far_);

  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(perspective_matrix));

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, kProjectionUniformBlockIndex,
                    projection_global_UBO_, 0, sizeof(glm::mat4));
  // Projection Matrix UBO END

  // Directional Light UBO
  glGenBuffers(1, &directional_light_global_UBO_);
  glBindBuffer(GL_UNIFORM_BUFFER, directional_light_global_UBO_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ECS::Components::DirectionalLight) * 4,
               nullptr, GL_STREAM_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, kDirectionalLightUniformBlockIndex,
                    directional_light_global_UBO_, 0,
                    sizeof(ECS::Components::DirectionalLight));
  // Directional Light UBO END

  // Point Lights UBO
  glGenBuffers(1, &point_light_global_UBO_);
  glBindBuffer(GL_UNIFORM_BUFFER, point_light_global_UBO_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ECS::Components::PointLight) * 8,
               nullptr, GL_STREAM_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, kPointLightsUniformBlockIndex,
                    point_light_global_UBO_, 0,
                    sizeof(ECS::Components::PointLight) * 8);
  // Point Lights UBO END
  printf("Global UBO Generated\n");
}

void RenderSystem::UpdatePerspectiveMatrix() {
  glm::mat4 perspective_matrix = glm::perspective(
      fov_, (float)viewport_width_ / (float)viewport_height_, z_near_, z_far_);
  glBindBuffer(GL_UNIFORM_BUFFER, projection_global_UBO_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(perspective_matrix));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::SetViewportSize(GLuint width, GLuint height) {
  viewport_width_ = width;
  viewport_height_ = height;
  UpdatePerspectiveMatrix();
  glViewport(0, 0, viewport_width_, viewport_height_);
  Draw();
}

void RenderSystem::SetFov(GLuint fov) {
  fov_ = fov;
  UpdatePerspectiveMatrix();
}

void RenderSystem::SetZ(GLfloat z_near, GLfloat z_far) {
  z_near_ = z_near;
  z_far_ = z_far;
  UpdatePerspectiveMatrix();
}

void RenderSystem::Update(float delta_time) { Draw(); }

void RenderSystem::Draw() {

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glm::mat4 camera_matrix = GetCameraMatrix();
  auto &coordinator = GetApplication().GetCoordinator();
  auto objects = coordinator.GetEntities<WE::ECS::Components::MeshRenderer,
                                         WE::ECS::Components::Transform,
                                         WE::ECS::Components::Material>();
  auto lights = coordinator.GetEntities<WE::ECS::Components::PointLight>();

  auto t1 = std::chrono::high_resolution_clock::now();
  RenderShadowMaps(lights, objects);
  auto t2 = std::chrono::high_resolution_clock::now();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                GetApplication().GetTextureManager().GetGaussianTermTexture());
  base_program_.UseProgram();
  glUniform1f(kShininess, 1.0f);
  glUniform1f(kFarPlaneUniformLocation, 200.0f);
  BindDirectionalLight();
  BindPointLights(lights);

  for (auto &object : objects) {

    auto &mesh_renderer =
        coordinator.GetComponent<ECS::Components::MeshRenderer>(object);
    auto &transform =
        coordinator.GetComponent<ECS::Components::Transform>(object);
    auto &material =
        coordinator.GetComponent<ECS::Components::Material>(object);

    BindUniforms(transform, camera_matrix);
    BindTextures(material);
    DrawMesh(mesh_renderer.mesh_id);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  base_program_.FreeProgram();
  auto t3 = std::chrono::high_resolution_clock::now();
  auto shadow_pass_duration =
      std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  auto render_pass_duration =
      std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
  printf("Shadow pass: %lld microseconds\nRender pass: %lld "
         "microseconds\n",
         shadow_pass_duration.count(), render_pass_duration.count());
  glfwSwapBuffers(GetApplication().GetWindow());
}

void RenderSystem::RenderShadowMaps(
    const std::vector<WE::ECS::Entity> &light_entities,
    const std::vector<WE::ECS::Entity> &object_entites) {

  shadow_map_program_.UseProgram();

  uint8_t maps_amount = std::min(light_entities.size(), (size_t)8);

  float aspect = (float)kShadowMapWidth / (float)kShadowMapHeight;
  float z_near = 0.01f;
  float z_far = 200.0f;
  glUniform1f(kFarPlaneUniformLocation, z_far);
  glm::mat4 shadow_projection =
      glm::perspective(glm::radians(90.0f), aspect, z_near, z_far);

  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer_);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  auto &coordinator = GetApplication().GetCoordinator();

  for (uint8_t i = 0; i < maps_amount; i++) {

    auto &light = coordinator.GetComponent<WE::ECS::Components::PointLight>(
        light_entities[i]);
    if (light.shadow_map == 0) {
      glGenTextures(1, &light.shadow_map);
      glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow_map);
      for (uint8_t k = 0; k < 6; k++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, 0, GL_DEPTH_COMPONENT,
                     kShadowMapWidth, kShadowMapHeight, 0, GL_DEPTH_COMPONENT,
                     GL_FLOAT, NULL);
      }
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow_map);
    glUniform3fv(kLightPositionUniformLocation, 1,
                 glm::value_ptr(light.position));
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light.shadow_map,
                         0);

    glViewport(0, 0, kShadowMapWidth, kShadowMapHeight);
    glClear(GL_DEPTH_BUFFER_BIT);
    std::array<glm::mat4, 6> shadow_transforms;
    shadow_transforms[0] =
        shadow_projection *
        glm::lookAt(light.position,
                    light.position + glm::vec3(1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f));
    shadow_transforms[1] =
        shadow_projection *
        glm::lookAt(light.position,
                    light.position + glm::vec3(-1.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f));
    shadow_transforms[2] =
        shadow_projection *
        glm::lookAt(light.position,
                    light.position + glm::vec3(0.0f, 1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
    shadow_transforms[3] =
        shadow_projection *
        glm::lookAt(light.position,
                    light.position + glm::vec3(0.0f, -1.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f));
    shadow_transforms[4] =
        shadow_projection *
        glm::lookAt(light.position,
                    light.position + glm::vec3(0.0f, 0.0f, 1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f));
    shadow_transforms[5] =
        shadow_projection *
        glm::lookAt(light.position,
                    light.position + glm::vec3(0.0f, 0.0f, -1.0f),
                    glm::vec3(0.0f, -1.0f, 0.0f));

    glUniformMatrix4fv(kShadowMatricesUniformLocation, 6, GL_FALSE,
                       glm::value_ptr(shadow_transforms[0]));

    for (auto object : object_entites) {
      auto &transform =
          coordinator.GetComponent<WE::ECS::Components::Transform>(object);
      auto &mesh_renderer =
          coordinator.GetComponent<WE::ECS::Components::MeshRenderer>(object);

      glm::mat4 model_world_matrix =
          glm::translate(glm::mat4(1.0f), transform.position) *
          glm::mat4_cast(transform.rotation) *
          glm::scale(glm::mat4(1.0f), transform.scale);
      glUniformMatrix4fv(kModelWorldSpaceUniformLocation, 1, GL_FALSE,
                         glm::value_ptr(model_world_matrix));
      DrawMesh(mesh_renderer.mesh_id);
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, viewport_width_, viewport_height_);
  shadow_map_program_.FreeProgram();
}

void RenderSystem::BindDirectionalLight() {

  auto &coordinator = GetApplication().GetCoordinator();
  auto directional_lights =
      coordinator.GetEntities<ECS::Components::DirectionalLight>();

  if (directional_lights.size() <= 0) {
    glUniform1i(kDirectionalLightExists, 0);
    return;
  } else {
    glUniform1i(kDirectionalLightExists, 1);
  }

  auto directional_light =
      coordinator.GetComponent<ECS::Components::DirectionalLight>(
          directional_lights[0]);

  directional_light.direction =
      glm::mat3(GetCameraMatrix()) * directional_light.direction;

  glBindBuffer(GL_UNIFORM_BUFFER, directional_light_global_UBO_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0,
                  sizeof(ECS::Components::DirectionalLight),
                  &directional_light);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::BindPointLights(std::vector<WE::ECS::Entity> &lights) {
  auto &coordinator = GetApplication().GetCoordinator();
  auto lights_amount = std::min(lights.size(), (size_t)8);
  glUniform1i(kAmountOfPointLights, lights_amount);

  glm::mat4 camera_matrix = GetCameraMatrix();

  glBindBuffer(GL_UNIFORM_BUFFER, point_light_global_UBO_);
  for (int i = 0; i < lights_amount; i++) {
    auto point_light =
        coordinator.GetComponent<ECS::Components::PointLight>(lights[i]);
    point_light.position =
        camera_matrix * glm::vec4(point_light.position, 1.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(ECS::Components::PointLight) * i,
                    sizeof(ECS::Components::PointLight), &point_light);
    glActiveTexture(GL_TEXTURE3 + i);
    glBindTexture(GL_TEXTURE_CUBE_MAP, point_light.shadow_map);
  }
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RenderSystem::BindUniforms(ECS::Components::Transform &transform,
                                glm::mat4 &camera_matrix) {

  glm::mat4 model_camera_matrix =
      camera_matrix * glm::translate(glm::mat4(1.0f), transform.position) *
      glm::mat4_cast(transform.rotation) *
      glm::scale(glm::mat4(1.0f), transform.scale);

  glm::mat3 model_camera_normal_matrix =
      glm::transpose(glm::inverse(glm::mat3(model_camera_matrix)));
  glm::mat3 camera_to_world_rotation_matrix =
      glm::inverse(glm::mat3(camera_matrix));

  glUniformMatrix4fv(kModelCameraUniformLocation, 1, GL_FALSE,
                     glm::value_ptr(model_camera_matrix));
  glUniformMatrix3fv(kModelCameraNormalUniformLocation, 1, GL_FALSE,
                     glm::value_ptr(model_camera_normal_matrix));
  glUniformMatrix3fv(kCameraToWorldRotationMatrixUniformLocation, 1, GL_FALSE,
                     glm::value_ptr(camera_to_world_rotation_matrix));
}

void RenderSystem::BindTextures(ECS::Components::Material &material) {

  auto &texture_manager = GetApplication().GetTextureManager();
  GLuint diffuse_texture =
      material.diffuse_texture_id == 0
          ? texture_manager.GetDefaultDiffuseTexture()
          : texture_manager.GetTexture(material.diffuse_texture_id);
  GLuint specular_texture =
      material.specular_texture_id == 0
          ? texture_manager.GetDefaultSpecularTexture()
          : texture_manager.GetTexture(material.specular_texture_id);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, diffuse_texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, specular_texture);
}

void RenderSystem::DrawMesh(MeshId mesh_id) {

  GLuint VAO = GetApplication().GetModelManager().GetVAO(mesh_id);
  GLuint elements_amount =
      GetApplication().GetModelManager().GetIndicesAmount(mesh_id);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, elements_amount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

glm::mat4 RenderSystem::GetCameraMatrix() {
  auto &camera = GetApplication().GetCamera();
  return glm::translate(glm::mat4(camera.rotation), camera.position);
}

} // namespace WE::Render
