#ifndef WE_ECS_COORDINATOR_H
#define WE_ECS_COORDINATOR_H

#include "ECS/component_manager.hpp"
#include "ECS/entity_manager.h"
#include "ECS/system_manager.hpp"
#include "input/input_manager.h"
#include <memory>
#include <vector>
namespace WE::ECS {

using EntityArray = std::vector<Entity>;

class Coordinator {

public:
  Coordinator() {
    component_manager = std::make_unique<ComponentManager>();
    entity_manager = std::make_unique<EntityManager>();
    system_manager = std::make_unique<SystemManager>();
  }

  inline Entity CreateEntity() { return entity_manager->CreateEntity(); }

  void DestroyEntity(Entity entity) {
    entity_manager->DestroyEntity(entity);
    component_manager->EntityDestroyed(entity);
  }

  template <typename T> void RegisterComponent() {
    component_manager->RegisterComponent<T>();
  }

  template <typename T> void AddComponent(Entity entity, T component) {
    component_manager->AddComponent(entity, component);

    auto signature = entity_manager->GetSignature(entity);
    signature.set(component_manager->GetComponentType<T>(), true);
    entity_manager->SetSignature(entity, signature);
  }

  template <typename T> void RemoveComponent(Entity entity) {
    component_manager->RemoveComponent<T>(entity);

    auto signature = entity_manager->GetSignature(entity);
    signature.set(component_manager->GetComponentType<T>(), false);
    entity_manager->SetSignature(entity, signature);
  }

  template <typename T> inline T &GetComponent(Entity entity) {
    return component_manager->GetComponent<T>(entity);
  }

  template <typename T> inline ComponentType GetComponentType() {
    return component_manager->GetComponentType<T>();
  }

  template <typename... Types> inline std::vector<Entity> GetEntities() const {
    auto components_signature = component_manager->GetSignature<Types...>();
    return GetEntities(components_signature);
  }

  inline EntityArray GetEntities(Signature components_signature) const {
    EntityArray entities{};
    entities.reserve(entity_manager->GetLastEntity() + 1);
    for (Entity entity = 0; entity <= entity_manager->GetLastEntity();
         entity++) {
      if (EntityHasComponents(entity, components_signature))
        entities.push_back(entity);
    }
    return entities;
  }

  inline bool EntityHasComponents(Entity entity,
                                  Signature components_signature) const {
    auto entity_signature = entity_manager->GetSignature(entity);
    return (entity_signature & components_signature) == components_signature;
  }

  template <typename T>
  inline std::shared_ptr<T> RegisterSystem(WE::Application &application) {
    return system_manager->RegisterSystem<T>(application);
  }

  inline void UpdateSystems(float delta_time) const {
    system_manager->UpdateSystems(delta_time);
  }

  Coordinator(Coordinator &&other) = default;
  Coordinator &operator=(Coordinator &&other) = default;

  Coordinator(const Coordinator &) = delete;
  Coordinator &operator=(const Coordinator &) = delete;

private:
  std::unique_ptr<ComponentManager> component_manager;
  std::unique_ptr<EntityManager> entity_manager;
  std::unique_ptr<SystemManager> system_manager;
};

} // namespace WE::ECS

#endif // !WE_ECS_COORDINATOR_H
