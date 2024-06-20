#ifndef WE_ECS_COMPONENTS_SPRITE_2D_H
#define WE_ECS_COMPONENTS_SPRITE_2D_H

#include "render/texture_manager.h"
namespace WE::ECS::Components {

struct Sprite2D {
  Render::TextureId sprite_id;
};

} // namespace WE::ECS::Components

#endif // !WE_ECS_COMPONENTS_SPRITE_2D_H
