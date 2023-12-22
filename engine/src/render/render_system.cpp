#include "render/render_system.h"

#include "ECS/components/directional_light.h"
#include "stb_image.h"
#include <cmath>
#include <cstddef>
#include <vector>
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

#include "ECS/components/material.h"
#include "ECS/components/mesh_renderer.h"
#include "ECS/components/transform.h"
#include "ECS/coordinator.h"
#include "application.h"
#include "render/program.h"
#include "render/texture_manager.h"

namespace {
const GLuint kProjectionUniformBlockIndex = 0;

const GLuint kModelCameraUniformLocation = 1;
const GLuint kModelCameraNormalUniformLocation = 2;

const GLuint kDirectionalLightDirection = 128;
const GLuint kDirectionalLightAmbient = 129;
const GLuint kDirectionalLightDiffuse = 130;
const GLuint kDirectionalLightSpecular = 131;
const GLuint kDirectionalLightExists = 132;
const GLuint kShininess = 133;

} // namespace

namespace WE::Render {

RenderSystem::RenderSystem() {

  glEnable(GL_CULL_FACE);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glCullFace(GL_FRONT_FACE);
  glFrontFace(GL_CW);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  stbi_set_flip_vertically_on_load(true);

  GenerateGlobalUBO();
  GenerateProgram();
}

void RenderSystem::GenerateProgram() {
  printf("Generating Program...\n");
  base_program_ =
      Program("./engine/shaders/shader.vert", "./engine/shaders/gaussian.frag");
  printf("Program Generated\n");
}

void RenderSystem::GenerateGlobalUBO() {
  printf("Generating Global UBO...\n");
  glGenBuffers(1, &projection_global_UBO_);
  glBindBuffer(GL_UNIFORM_BUFFER, projection_global_UBO_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_STREAM_DRAW);

  glm::mat4 perspective_matrix = glm::perspective(
      fov_, (float)viewport_width_ / (float)viewport_height_, z_near_, z_far_);

  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                  glm::value_ptr(perspective_matrix));

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, kProjectionUniformBlockIndex,
                    projection_global_UBO_, 0, sizeof(glm::mat4) * 2);
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
  auto &coordinator = GetApplication()->GetCoordinator();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                GetApplication()->GetTextureManager().GetGaussianTermTexture());
  base_program_.UseProgram();
  auto entities =
      coordinator
          .GetEntities<ECS::Components::MeshRenderer,
                       ECS::Components::Transform, ECS::Components::Material>();
  for (auto &entity : entities) {

    auto &mesh_renderer =
        coordinator.GetComponent<ECS::Components::MeshRenderer>(entity);
    auto &transform =
        coordinator.GetComponent<ECS::Components::Transform>(entity);
    auto &material =
        coordinator.GetComponent<ECS::Components::Material>(entity);

    BindUniforms(transform, camera_matrix);
    BindTextures(material);
    DrawMesh(mesh_renderer.mesh_id);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  base_program_.FreeProgram();

  glfwSwapBuffers(GetApplication()->GetWindow());
}

void RenderSystem::BindDirectionalLight(
    ECS::Components::DirectionalLight &directional_light) {
  glm::vec3 direction =
      glm::mat3(GetCameraMatrix()) * directional_light.direction;
  glUniform3fv(kDirectionalLightDirection, 1, glm::value_ptr(direction));
  glUniform3fv(kDirectionalLightAmbient, 1,
               glm::value_ptr(directional_light.ambient));
  glUniform3fv(kDirectionalLightDiffuse, 1,
               glm::value_ptr(directional_light.diffuse));
  glUniform3fv(kDirectionalLightSpecular, 1,
               glm::value_ptr(directional_light.specular));
}

void RenderSystem::BindUniforms(ECS::Components::Transform &transform,
                                glm::mat4 &camera_matrix) {

  glm::mat4 model_camera_matrix =
      camera_matrix * glm::translate(glm::mat4(1.0f), transform.position) *
      glm::mat4_cast(transform.rotation) *
      glm::scale(glm::mat4(1.0f), transform.scale);

  glm::mat3 model_camera_normal_matrix =
      glm::transpose(glm::inverse(glm::mat3(model_camera_matrix)));

  glUniformMatrix4fv(kModelCameraUniformLocation, 1, GL_FALSE,
                     glm::value_ptr(model_camera_matrix));
  glUniformMatrix3fv(kModelCameraNormalUniformLocation, 1, GL_FALSE,
                     glm::value_ptr(model_camera_normal_matrix));
}

void RenderSystem::BindTextures(ECS::Components::Material &material) {

  auto &texture_manager = GetApplication()->GetTextureManager();
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

  GLuint VAO = GetApplication()->GetModelManager().GetVAO(mesh_id);
  GLuint elements_amount =
      GetApplication()->GetModelManager().GetIndicesAmount(mesh_id);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, elements_amount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

glm::mat4 RenderSystem::GetCameraMatrix() {
  auto &camera = GetApplication()->GetCamera();
  return glm::translate(glm::mat4(camera.rotation), camera.position);
}

} // namespace WE::Render
