#ifndef WE_APPLICATION_H
#define WE_APPLICATION_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/common.hpp>

#include "ECS/coordinator.h"
#include "application_window.h"
#include "input/input_manager.h"
#include "render/i_pipeline.h"
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
  Application() = delete;
  Application(Render::IPipeline &pipeline) : Application(pipeline, 800, 600) {}
  Application(Render::IPipeline &pipeline, int window_width, int window_height);

  void Start();

  inline ApplicationWindow &GetWindow() { return application_window_; }
  inline Render::IPipeline &GetPipeline() { return pipeline_; }
  inline ECS::Coordinator &GetCoordinator() { return coordinator_; }
  inline Input::InputManager &GetInputManager() { return input_manager_; }

  inline void SetWindowSize(int width, int height) {
    GetWindow().SetWindowSize(width, height);
  }
  inline void WindowResizeCallback(int width, int height) {
    GetPipeline().SetViewportSize(width, height);
  }
  inline void KeyCallback(Input::Key key, Input::KeyState key_state) {
    GetInputManager().SetKeyState(key, key_state);
  }
  inline void CursorPositionCallback(glm::vec2 mouse_position) {
    GetInputManager().SetMousePosition(mouse_position);
  }
  inline void MouseButtonCallback(Input::Key key, Input::KeyState key_state) {
    GetInputManager().SetMouseKeyState(key, key_state);
  }

  Application(Application &&other) = default;
  Application &operator=(Application &&other) = delete;

  Application(const Application &) = delete;
  Application &operator=(const Application &) = delete;

private:
  float delta_time_ = 0.0f;
  ECS::Coordinator coordinator_;

  Input::InputManager input_manager_;
  Render::IPipeline &pipeline_;
  ApplicationWindow application_window_;
};

} // namespace WE

#endif // !WE_APPLICATION_H
