#ifndef WE_ECS_SYSTEM_MANAGER_HPP
#define WE_ECS_SYSTEM_MANAGER_HPP

#include "ECS/i_system.h"
#include <memory>
#include <unordered_map>
namespace WE::ECS {
class SystemManager {
public:
  template <typename T>
  std::shared_ptr<T> RegisterSystem(WE::Application &application) {
    const char *type_name = typeid(T).name();
    auto system = std::make_shared<T>(application);
    systems.insert({type_name, system});
    return system;
  }

  void UpdateSystems(float delta_time) {
    for (auto system : systems) {
      system.second->Update(delta_time);
    }
  }

private:
  std::unordered_map<const char *, std::shared_ptr<ISystem>> systems{};
};
} // namespace WE::ECS

#endif // !WE_ECS_SYSTEM_MANAGER_HPP
