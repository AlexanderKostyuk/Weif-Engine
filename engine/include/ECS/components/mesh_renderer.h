#ifndef WE_ECS_COMPONENTS_MESH_RENDERER_H
#define WE_ECS_COMPONENTS_MESH_RENDERER_H

#include "render/model_manager.h"

namespace WE::ECS::Components {
struct MeshRenderer {
  Render::MeshId mesh_id;
};
} // namespace WE::ECS::Components
#endif // !WE_ECS_COMPONENTS_MESH_H
