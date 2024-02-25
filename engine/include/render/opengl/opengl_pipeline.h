#ifndef WE_RENDER_OPENGL_PIPELINE_H
#define WE_RENDER_OPENGL_PIPELINE_H

#include "render/i_pipeline.h"
#include "render/model_manager.h"
#include "render/opengl/program.h"
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

  Program base_program_;
  Program shadow_map_program_;
  Program shadow_map_directional_program_;

  GLuint shadow_map_framebuffer_;
  GLuint color_framebuffer_;

  GLuint projection_UBO_;
  GLuint directional_light_UBO_;
  GLuint point_light_UBO_;
  GLuint shadow_map_UBO_;
};

} // namespace WE::Render::Opengl

#endif // !WE_RENDER_OPENGL_PIPELINE_H
