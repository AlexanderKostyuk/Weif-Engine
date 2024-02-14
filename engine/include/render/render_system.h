
#ifndef WE_RENDER_RENDER_SYSTEM_H
#define WE_RENDER_RENDER_SYSTEM_H

#include "ECS/i_system.h"

namespace WE::Render {

class RenderSystem : public ECS::ISystem {
private:
public:
  RenderSystem(WE::Application &application);
  void Update(float delta_time) override;
};
} // namespace WE::Render

#endif // !WE_RENDER_SYSTEM_H
