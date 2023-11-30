#ifndef WE_ECS_SYSTEM_MANAGER_HPP
#define WE_ECS_SYSTEM_MANAGER_HPP

#include "ECS/entity_manager.h"
#include "ECS/i_system.h"
#include <memory>
#include <unordered_map>
namespace WE::ECS {
class SystemManager {
public:
  template <typename T>
  std::shared_ptr<T> RegisterSystem(Application *application) {
    const char *type_name = typeid(T).name();
    auto system = std::make_shared<T>();
    system->SetApplication(application);
    systems.insert({type_name, system});
    return system;
  }

  template <typename T> void SetSignature(Signature signature) {
    const char *type_name = typeid(T).name();
    signatures.insert({type_name, signature});
  }

  void EntityDestroyed(Entity entity) {
    for (auto const &pair : systems) {
      auto const &system = pair.second;
      system->entities.erase(entity);
    }
  }

  void EntitySignatureChanged(Entity entity, Signature entity_signature) {
    for (auto const &pair : systems) {
      auto const &type = pair.first;
      auto const &system = pair.second;
      auto const &system_signature = signatures[type];
      if (system_signature == 0)
        continue;

      if ((entity_signature & system_signature) == system_signature)
        system->entities.insert(entity);
      else
        system->entities.erase(entity);
    }
  }

  void UpdateSystems(float delta_time) {
    for (auto system : systems) {
      system.second->Update(delta_time);
    }
  }

private:
  std::unordered_map<const char *, Signature> signatures{};
  std::unordered_map<const char *, std::shared_ptr<ISystem>> systems{};
};
} // namespace WE::ECS

#endif // !WE_ECS_SYSTEM_MANAGER_HPP
