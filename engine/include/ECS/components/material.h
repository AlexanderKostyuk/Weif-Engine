#ifndef WE_ESC_COMPONENTS_MATERIAL_H
#define WE_ESC_COMPONENTS_MATERIAL_H

#include "render/texture_manager.h"

namespace WE::ECS::Components {

struct Material {

  Render::TextureId diffuse_texture_id = 0;
  Render::TextureId specular_texture_id = 0;
  float shininess;
};

} // namespace WE::ECS::Components

#endif // !WE_ESC_COMPONENTS_MATERIAL_H
