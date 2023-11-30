#ifndef WE_APPLICATION_H
#define WE_APPLICATION_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <memory>

#include "ECS/coordinator.h"
#include "input/input_manager.h"
#include "render/camera.h"
#include "render/model_manager.h"
#include "render/render_system.h"
#include "render/texture_manager.h"
namespace WE {

class Application {
private:
  void Init();
  void Loop();
  void Cleanup();

  void InitWindow();

  void InitComponents();
  void InitSystems();

public:
  Application();
  Application(int window_width, int window_height);

  void Start();
  void SetWindowSize(GLuint width, GLuint height);

  inline GLFWwindow *GetWindow() { return window_; }

  inline ECS::Coordinator &GetCoordinator() { return coordinator_; }
  inline Input::InputManager &GetInputManager() { return input_manager_; }
  inline Render::ModelManager &GetModelManager() { return model_manager_; }
  inline Render::TextureManager &GetTextureManager() {
    return texture_manager_;
  }

  inline Render::Camera &GetCamera() { return camera_; }

  Application(Application &&other) = default;
  Application &operator=(Application &&other) = delete;

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

private:
  int window_width_;
  int window_height_;
  GLFWwindow *window_;
  float delta_time_;
  ECS::Coordinator coordinator_;

  Input::InputManager input_manager_;
  Render::ModelManager model_manager_;
  Render::TextureManager texture_manager_;
  Render::Camera camera_;

  std::shared_ptr<Render::RenderSystem> render_system_;
};

} // namespace WE

#endif // !WE_APPLICATION_H
