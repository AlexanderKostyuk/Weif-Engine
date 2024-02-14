#ifndef WE_RENDER_OBJECT_H
#define WE_RENDER_OBJECT_H

#include "glm/glm.hpp"

#include "render/model_manager.h"
#include "render/texture_manager.h"
namespace WE::Render {
struct Object {
  glm::mat4 transform;
  TextureId diffuse_texture_id;
  TextureId specular_texture_id;
  MeshId mesh_id;
};
} // namespace WE::Render

#endif // !WE_RENDER_OBJECT_H
