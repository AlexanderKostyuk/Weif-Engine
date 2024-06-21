#ifndef WE_RENDER_OBJECT_H
#define WE_RENDER_OBJECT_H

#include "render/model_manager.h"
#include "render/texture_manager.h"
#include <cstddef>
#include <functional>

namespace WE::Render {
struct Object {
  TextureId diffuse_texture_id;
  TextureId specular_texture_id;
  MeshId mesh_id;

  inline bool operator==(const Object &object) const {
    return diffuse_texture_id == object.diffuse_texture_id &&
           specular_texture_id == object.specular_texture_id &&
           mesh_id == object.mesh_id;
  }
};

} // namespace WE::Render

template <> struct std::hash<WE::Render::Object> {
  std::size_t operator()(const WE::Render::Object &o) const {
    return ((std::hash<WE::Render::TextureId>()(o.diffuse_texture_id) ^
             (std::hash<WE::Render::TextureId>()(o.specular_texture_id)
              << 1)) >>
            1) ^
           (std::hash<WE::Render::MeshId>()(o.mesh_id) << 1);
  }
};
#endif // !WE_RENDER_OBJECT_H
