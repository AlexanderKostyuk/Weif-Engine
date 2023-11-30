#ifndef WE_ECS_I_SYSTEM_H
#define WE_ECS_I_SYSTEM_H

#include "ECS/entity_manager.h"
#include <set>
namespace WE {
class Application;
}

namespace WE::ECS {

class ISystem {

public:
  virtual void Update(float delta_time) = 0;
  virtual ~ISystem() = default;
  std::set<Entity> entities;

  inline WE::Application *GetApplication() { return application_; }

  inline void SetApplication(WE::Application *application) {
    application_ = application;
  }

private:
  WE::Application *application_;
};
} // namespace WE::ECS

#endif // !WE_ECS_I_SYSTEM_H
