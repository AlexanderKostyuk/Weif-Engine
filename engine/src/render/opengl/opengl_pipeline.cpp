#include "render/opengl/opengl_pipeline.h"

#include "GL/gl3w.h"
#include "GL/glcorearb.h"
#include "glm/ext.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/matrix.hpp"
#include <algorithm>
#include <cstddef>
#include <gl/gl.h>
#include <memory>

#include "ECS/components/point_light.h"
#include "application.h"
#include "render/lighting/light_info.h"
#include "render/lighting/point_light.h"
#include "render/opengl/global_varibales.h"
#include "render/opengl/program.h"
#include "render/opengl/renderer_2d.h"

namespace {

void GenerateUBO(GLuint &UBO, const std::size_t size,
                 const GLuint block_index) {
  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STREAM_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, block_index, UBO, 0, size);
}

void UniformBufferSubData(const GLuint UBO, const std::size_t offset,
                          const std::size_t size, void *const data) {
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  if (severity <= GL_DEBUG_SEVERITY_NOTIFICATION)
    return;
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

} // namespace

namespace WE::Render::Opengl {

void OpenglPipeline::Init() {
  gl3wInit();
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT_FACE);
  glFrontFace(GL_CW);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  glGenFramebuffers(1, &shadow_map_framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer_);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  texture_manager_.Init();
  renderer_2d_ = std::make_unique<Renderer2D>();
  renderer_3d_ = std::make_unique<Renderer3D>();
  GenerateUBOs();
  GeneratePrograms();
}

void OpenglPipeline::GeneratePrograms() {

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);
  glEnableVertexAttribArray(5);
  glEnableVertexAttribArray(6);
  light_pass_program_ =
      Program("./engine/shaders/shader.vert", "./engine/shaders/gaussian.frag");

  point_shadow_pass_program_ =
      Program("./engine/shaders/shadows/point_light.vert",
              "./engine/shaders/shadows/point_light.frag",
              "./engine/shaders/shadows/point_light.geom");

  directional_shadow_pass_program_ =
      Program("./engine/shaders/shadows/directional_light.vert",
              "./engine/shaders/shadows/directional_light.frag");
}

void OpenglPipeline::GenerateUBOs() {

  GenerateUBO(projection_UBO_, MAT4_SIZE * 3, kProjectionUniformBlockIndex);
  GenerateUBO(directional_light_UBO_, sizeof(WE::Render::Lighting::LightInfo),
              kDirectionalLightUniformBlockIndex);
  GenerateUBO(point_light_UBO_, sizeof(ECS::Components::PointLight) * 8,
              kPointLightsUniformBlockIndex);
  GenerateUBO(shadow_map_UBO_, MAT4_SIZE * 7, kShadowMapUniformBlockIndex);

  auto window_size = GetApplication()->GetWindow().GetWindowSize();

  // Populate Projection UBO
  glm::mat4 perspective_matrix = glm::perspective(
      fov_, (float)window_size.x / (float)window_size.y, z_near_, z_far_);

  UniformBufferSubData(projection_UBO_, 0, MAT4_SIZE,
                       glm::value_ptr(perspective_matrix));

  glm::mat4 ortho_projection =
      glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);

  UniformBufferSubData(shadow_map_UBO_, 0, MAT4_SIZE,
                       glm::value_ptr(ortho_projection));

  glm::mat4 shadow_projection =
      glm::perspective(glm::radians(90.0f), 1.0f, z_near_, z_far_);
  std::array<glm::mat4, 6> shadow_transforms;
  shadow_transforms[0] =
      shadow_projection * glm::lookAt(glm::vec3(0.0f),
                                      glm::vec3(1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
  shadow_transforms[1] =
      shadow_projection * glm::lookAt(glm::vec3(0.0f),
                                      glm::vec3(-1.0f, 0.0f, 0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
  shadow_transforms[2] =
      shadow_projection * glm::lookAt(glm::vec3(0.0f),
                                      glm::vec3(0.0f, 1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f));
  shadow_transforms[3] =
      shadow_projection * glm::lookAt(glm::vec3(0.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f),
                                      glm::vec3(0.0f, 0.0f, -1.0f));
  shadow_transforms[4] =
      shadow_projection * glm::lookAt(glm::vec3(0.0f),
                                      glm::vec3(0.0f, 0.0f, 1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));
  shadow_transforms[5] =
      shadow_projection * glm::lookAt(glm::vec3(0.0f),
                                      glm::vec3(0.0f, 0.0f, -1.0f),
                                      glm::vec3(0.0f, -1.0f, 0.0f));

  UniformBufferSubData(shadow_map_UBO_, MAT4_SIZE, MAT4_SIZE * 6,
                       glm::value_ptr(shadow_transforms[0]));
}

void OpenglPipeline::ShadowPass() {
  GenerateDirectionalLightShadowMap();
  GeneratePointLightShadowMaps();
}

void OpenglPipeline::GenerateDirectionalLightShadowMap() {

  if (!directional_light_.has_value())
    return;

  directional_shadow_pass_program_.UseProgram();
  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer_);
  SetViewportSize(shadow_resolution_, shadow_resolution_);

  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 direction = glm::vec3(directional_light_->direction);
  if (glm::dot(up, glm::normalize(direction)) == 1.0f)
    up = glm::vec3(-1.0f, 0.0f, 0.0f);
  glm::mat4 light_view = glm::lookAt(glm::vec3(0.0f), direction, up);

  UniformBufferSubData(projection_UBO_, MAT4_SIZE * 2, MAT4_SIZE,
                       glm::value_ptr(light_view));

  GLuint shadow_map = GetTexture(directional_light_->shadow_map_texture);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         shadow_map, 0);

  glClear(GL_DEPTH_BUFFER_BIT);

  for (auto object : objects_3d_) {

    ModelInfo model_info{
        .vao = GetModelManager().GetVAO(object.first.mesh_id),
        .transform_buffer =
            GetModelManager().GetTransfromBuffer(object.first.mesh_id),
        .elements_amount =
            GetModelManager().GetIndicesAmount(object.first.mesh_id)};
    model_info.diffuse_texture =
        object.first.diffuse_texture_id == 0
            ? GetTextureManager().GetTexture(
                  GetTextureManager().GetDefaultDiffuseTextureId())
            : GetTextureManager().GetTexture(object.first.diffuse_texture_id);
    model_info.specular_texture =
        object.first.specular_texture_id == 0
            ? GetTextureManager().GetTexture(
                  GetTextureManager().GetDefaultSpecularTextureId())
            : GetTextureManager().GetTexture(object.first.specular_texture_id);
    renderer_3d_->RenderObjects(object.second, model_info);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  auto window_size = GetApplication()->GetWindow().GetWindowSize();
  SetViewportSize(window_size.x, window_size.y);
  directional_shadow_pass_program_.FreeProgram();
}

void OpenglPipeline::GeneratePointLightShadowMaps() {

  if (point_lights_.empty())
    return;

  point_shadow_pass_program_.UseProgram();
  glUniform1f(kShadowPassFarPlaneLocation, z_far_);

  glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_framebuffer_);
  SetViewportSize(shadow_resolution_, shadow_resolution_);

  for (auto point_light : point_lights_) {
    auto shadow_map = GetTexture(point_light.shadow_map_texture);
    glUniform3fv(kLightPositionLocation, 1,
                 glm::value_ptr(point_light.position));
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);
    glClear(GL_DEPTH_BUFFER_BIT);

    for (auto object : objects_3d_) {

      ModelInfo model_info{
          .vao = GetModelManager().GetVAO(object.first.mesh_id),
          .transform_buffer =
              GetModelManager().GetTransfromBuffer(object.first.mesh_id),
          .elements_amount =
              GetModelManager().GetIndicesAmount(object.first.mesh_id)};
      model_info.diffuse_texture =
          object.first.diffuse_texture_id == 0
              ? GetTextureManager().GetTexture(
                    GetTextureManager().GetDefaultDiffuseTextureId())
              : GetTextureManager().GetTexture(object.first.diffuse_texture_id);
      model_info.specular_texture =
          object.first.specular_texture_id == 0
              ? GetTextureManager().GetTexture(
                    GetTextureManager().GetDefaultSpecularTextureId())
              : GetTextureManager().GetTexture(
                    object.first.specular_texture_id);
      renderer_3d_->RenderObjects(object.second, model_info);
    }
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  auto window_size = GetApplication()->GetWindow().GetWindowSize();
  SetViewportSize(window_size.x, window_size.y);
  point_shadow_pass_program_.FreeProgram();
}

void OpenglPipeline::LightPass() {

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  light_pass_program_.UseProgram();
  glUniform1f(kFarPlaneLocation, z_far_);
  auto gaussian_texture_id = GetTextureManager().GetGaussianTermTextureId();
  if (gaussian_texture_id != 0) {
    glActiveTexture(kGaussianTextureBinding);
    glBindTexture(GL_TEXTURE_2D, GetTexture(gaussian_texture_id));
  }
  BindDirectionalLight();
  BindPointLights();
  auto camera_transform = GetCameraMatrix();
  UniformBufferSubData(projection_UBO_, MAT4_SIZE, MAT4_SIZE,
                       glm::value_ptr(camera_transform));
  for (auto object : objects_3d_) {

    ModelInfo model_info{
        .vao = GetModelManager().GetVAO(object.first.mesh_id),
        .transform_buffer =
            GetModelManager().GetTransfromBuffer(object.first.mesh_id),
        .elements_amount =
            GetModelManager().GetIndicesAmount(object.first.mesh_id)};
    model_info.diffuse_texture =
        object.first.diffuse_texture_id == 0
            ? GetTextureManager().GetTexture(
                  GetTextureManager().GetDefaultDiffuseTextureId())
            : GetTextureManager().GetTexture(object.first.diffuse_texture_id);
    model_info.specular_texture =
        object.first.specular_texture_id == 0
            ? GetTextureManager().GetTexture(
                  GetTextureManager().GetDefaultSpecularTextureId())
            : GetTextureManager().GetTexture(object.first.specular_texture_id);
    renderer_3d_->RenderObjects(object.second, model_info);
  }
  light_pass_program_.FreeProgram();

  for (auto pack : objects_2d_) {
    renderer_2d_->RenderSprites(pack.second, GetTexture(pack.first));
  }
}

void OpenglPipeline::BindDirectionalLight() {
  if (!directional_light_.has_value()) {
    glUniform1i(kDirectionalLightExistsLocation, 0);
    return;
  }
  glUniform1i(kDirectionalLightExistsLocation, 1);

  auto camera_rotation = glm::mat3(GetCamera().rotation);
  auto direction = camera_rotation * directional_light_->direction;
  glUniform3fv(kDirectionalLightCameraSpaceDirectionLocation, 1,
               glm::value_ptr(direction));
  UniformBufferSubData(directional_light_UBO_, 0,
                       sizeof(WE::Render::Lighting::LightInfo),
                       &(directional_light_->light_info));

  glActiveTexture(kDirectionalShadowMapBinding);
  auto shadow_map = GetTexture(directional_light_->shadow_map_texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map);
}

void OpenglPipeline::BindPointLights() {
  auto lights_amount = std::min(point_lights_.size(), (size_t)8);
  glUniform1i(kAmountOfPointLightsLocation, lights_amount);
  if (lights_amount == 0)
    return;

  auto camera_transform = GetCameraMatrix();
  auto point_light_byte_size =
      VEC4_SIZE + sizeof(WE::Render::Lighting::LightInfo) + VEC4_SIZE;

  for (int i = 0; i < lights_amount; i++) {
    glUniform3fv(kPointLightPositionLocation + i, 1,
                 glm::value_ptr(point_lights_[i].position));
    auto position = camera_transform * point_lights_[i].position;

    UniformBufferSubData(point_light_UBO_, point_light_byte_size * i, VEC4_SIZE,
                         glm::value_ptr(position));
    UniformBufferSubData(
        point_light_UBO_, point_light_byte_size * i + VEC4_SIZE,
        sizeof(WE::Render::Lighting::LightInfo), &point_lights_[i].light_info);
    UniformBufferSubData(point_light_UBO_,
                         point_light_byte_size * (i + 1) - VEC4_SIZE, VEC4_SIZE,
                         glm::value_ptr(point_lights_[i].intensity));

    auto shadow_map = GetTexture(point_lights_[i].shadow_map_texture);
    glActiveTexture(kPointShadowMapBinding + i);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadow_map);
  }
}

void OpenglPipeline::DrawObject(const MeshId mesh_id) {
  GLuint VAO = GetVAO(mesh_id);
  GLuint elements_amount = GetElementsAmount(mesh_id);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, elements_amount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void OpenglPipeline::PostProcess() {}
void OpenglPipeline::Swap() { GetApplication()->GetWindow().SwapBuffers(); }

} // namespace WE::Render::Opengl
