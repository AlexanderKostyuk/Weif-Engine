#ifndef WE_ESC_COMPONENTS_MATERIAL_H
#define WE_ESC_COMPONENTS_MATERIAL_H

#include "render/texture_manager.h"

namespace WE::ECS::Components {

struct Material {

  Render::TextureId albedo_texture_id;
  Render::TextureId normal_texture_id;
};

} // namespace WE::ECS::Components

#endif // !WE_ESC_COMPONENTS_MATERIAL_H
