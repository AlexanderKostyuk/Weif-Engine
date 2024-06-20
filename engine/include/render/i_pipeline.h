#ifndef WE_RENDER_I_PIPELINE_H
#define WE_RENDER_I_PIPELINE_H

#include "GL/glcorearb.h"
#include <optional>
#include <unordered_map>
#include <vector>

#include "glm/ext/vector_int2.hpp"
#include "render/camera.h"
#include "render/lighting/directional_light.h"
#include "render/lighting/point_light.h"
#include "render/model_manager.h"
#include "render/object.h"

namespace WE {
class Application;
}

namespace WE::Render {

class IPipeline {
public:
  inline void Draw() {
    ShadowPass();
    LightPass();
    PostProcess();
    Swap();
  }

  inline void
  SetDirectionalLight(Lighting::DirectionalLight directional_light) {
    directional_light_ = directional_light;
  }
  inline void SetDirectionalLight() { directional_light_.reset(); }
  inline void SetPointLights(std::vector<Lighting::PointLight> point_lights) {
    point_lights_ = point_lights;
  }
  inline void SetObjects(const std::vector<Object> &&objects) {
    objects_ = objects;
  }
  inline void SetObjects2D(
      const std::unordered_map<TextureId, std::vector<glm::mat4>> &&objects) {
    objects_2d_ = objects;
  }
  inline void SetApplication(WE::Application *application) {
    application_ = application;
  }

  inline ModelManager &GetModelManager() { return model_manager_; }
  inline TextureManager &GetTextureManager() { return texture_manager_; }
  inline Camera &GetCamera() { return camera_; }
  inline WE::Application *GetApplication() { return application_; }
  inline std::uint16_t GetShadowSize() const { return shadow_resolution_; }
  inline glm::mat4 GetCameraMatrix() {
    return glm::translate(glm::mat4(camera_.rotation), camera_.position);
  }

  virtual void Init() = 0;
  virtual void SetViewportSize(int width, int height) = 0;
  inline void SetViewportSize(glm::ivec2 size) {
    SetViewportSize(size.x, size.y);
  }

protected:
  virtual void ShadowPass() = 0;
  virtual void LightPass() = 0;
  virtual void PostProcess() = 0;
  virtual void Swap() = 0;

protected:
  std::optional<Lighting::DirectionalLight> directional_light_;
  std::vector<Lighting::PointLight> point_lights_;
  std::vector<Object> objects_;
  std::unordered_map<TextureId, std::vector<glm::mat4>> objects_2d_;

  ModelManager model_manager_;
  TextureManager texture_manager_;
  Camera camera_;
  WE::Application *application_;

  float fov_ = 90.0f;
  std::uint16_t shadow_resolution_ = 8192;
  float z_near_ = 0.01f;
  float z_far_ = 100.0f;
};

} // namespace WE::Render

#endif // !WE_RENDER_I_PIPELINE_H
