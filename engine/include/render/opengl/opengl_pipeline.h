#ifndef WE_RENDER_OPENGL_PIPELINE_H
#define WE_RENDER_OPENGL_PIPELINE_H

#include "render/i_pipeline.h"
#include "render/model_manager.h"
#include "render/opengl/program.h"
#include "render/opengl/renderer_2d.h"
#include "render/texture_manager.h"
#include <memory>

namespace WE::Render::Opengl {

class OpenglPipeline : public IPipeline {
private:
  void GeneratePrograms();
  void GenerateUBOs();
  void GenerateDirectionalLightShadowMap();
  void BindDirectionalLight();
  void GeneratePointLightShadowMaps();
  void BindPointLights();
  void UpdatePerspectiveMatrix();
  void DrawObject(const MeshId mesh_id);

  inline GLuint GetVAO(const MeshId mesh_id) const {
    return model_manager_.GetVAO(mesh_id);
  }
  inline GLuint GetElementsAmount(const MeshId mesh_id) const {
    return model_manager_.GetIndicesAmount(mesh_id);
  }
  inline GLuint GetTexture(const TextureId texture_id) const {
    return texture_manager_.GetTexture(texture_id);
  }

  inline void SetViewportSize(int width, int height) override {
    glViewport(0, 0, width, height);
  }

  void Init() override;

protected:
  void ShadowPass() override;
  void LightPass() override;
  void PostProcess() override;
  void Swap() override;

private:
  GLfloat fov_ = 45.0f;
  GLfloat z_near_ = 0.1f;
  GLfloat z_far_ = 100.0f;

  std::unique_ptr<Renderer2D> renderer_2d_;

  Program light_pass_program_;
  Program point_shadow_pass_program_;
  Program directional_shadow_pass_program_;

  GLuint shadow_map_framebuffer_;
  GLuint color_framebuffer_;

  GLuint projection_UBO_;
  GLuint directional_light_UBO_;
  GLuint point_light_UBO_;
  GLuint shadow_map_UBO_;
};

} // namespace WE::Render::Opengl

#endif // !WE_RENDER_OPENGL_PIPELINE_H
