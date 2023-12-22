#ifndef WE_ECS_COMPONENT_MANAGER_HPP
#define WE_ECS_COMPONENT_MANAGER_HPP

#include "ECS/component_array.hpp"
#include "ECS/entity_manager.h"
#include <memory>
#include <unordered_map>
namespace WE::ECS {

class ComponentManager {
public:
  template <typename T> void RegisterComponent() {
    const char *type_name = typeid(T).name();

    component_types.insert({type_name, next_component_type});
    component_arrays.insert({type_name, std::make_shared<ComponentArray<T>>()});
    next_component_type++;
  }

  template <typename T> ComponentType GetComponentType() const {
    const char *type_name = typeid(T).name();
    return component_types.at(type_name);
  }

  template <typename T> Signature GetSignature() const {
    auto component_type = GetComponentType<T>();
    Signature signature{};
    signature.set(component_type, true);
    return signature;
  }

  template <typename T, typename V, typename... Types>
  Signature GetSignature() const {
    Signature signature{};
    signature |= GetSignature<T>() | GetSignature<V, Types...>();
    return signature;
  }

  template <typename T> void AddComponent(Entity entity, T component) {
    GetComponentArray<T>()->InsertData(entity, component);
  }

  template <typename T> void RemoveComponent(Entity entity) {
    GetComponentArray<T>()->RemoveData(entity);
  }

  template <typename T> T &GetComponent(Entity entity) {
    return GetComponentArray<T>()->GetData(entity);
  }

  template <typename T> T &GetComponentByIndex(std::size_t index) {
    return GetComponentArray<T>()->GetDataByIndex(index);
  }

  template <typename T> std::size_t GetComponentsAmount() {
    return GetComponentArray<T>()->GetComponentsAmount();
  }

  void EntityDestroyed(Entity entity) {
    for (auto const &pair : component_arrays) {
      auto const &component = pair.second;
      component->EntityDestroyed(entity);
    }
  }

private:
  std::unordered_map<const char *, ComponentType> component_types{};
  std::unordered_map<const char *, std::shared_ptr<IComponentArray>>
      component_arrays{};
  ComponentType next_component_type;

  template <typename T> std::shared_ptr<ComponentArray<T>> GetComponentArray() {
    const char *typeName = typeid(T).name();
    return std::static_pointer_cast<ComponentArray<T>>(
        component_arrays.at(typeName));
  }
};

} // namespace WE::ECS

#endif // !WE_ECS_COMPONENT_MANAGER_HPP
