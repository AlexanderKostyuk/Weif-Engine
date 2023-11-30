#ifndef WE_RENDER_CAMERA_H
#define WE_RENDER_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
namespace WE::Render {

struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
};

} // namespace WE::Render

#endif // !WE_CAMERA_H
