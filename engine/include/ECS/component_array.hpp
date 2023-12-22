#ifndef WE_ECS_COMPONENT_ARRAY_HPP
#define WE_ECS_COMPONENT_ARRAY_HPP

#include <array>
#include <cstddef>
#include <unordered_map>

#include "ECS/entity_manager.h"

namespace WE::ECS {

class IComponentArray {
public:
  virtual ~IComponentArray() = default;
  virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T> class ComponentArray : public IComponentArray {
public:
  void InsertData(Entity entity, T component) {
    std::size_t new_index = size;
    entity_to_index_map[entity] = new_index;
    index_to_entity_map[new_index] = entity;
    component_array[new_index] = component;
    size++;
  }

  void RemoveData(Entity entity) {
    size_t index_of_removed_entity = entity_to_index_map[entity];
    size_t index_of_last_element = size - 1;
    component_array[index_of_removed_entity] =
        component_array[index_of_last_element];

    Entity entity_of_last_element = index_to_entity_map[index_of_last_element];
    entity_to_index_map[entity_of_last_element] = index_of_removed_entity;
    index_to_entity_map[index_of_removed_entity] = entity_of_last_element;
    entity_to_index_map.erase(entity);
    index_to_entity_map.erase(index_of_last_element);
    --size;
  }

  inline T &GetData(Entity entity) {
    return component_array[entity_to_index_map.at(entity)];
  }

  inline T &GetDataByIndex(std::size_t index) { return component_array[index]; }

  inline std::size_t GetComponentsAmount() { return size; }

  void EntityDestroyed(Entity entity) override {
    if (entity_to_index_map.find(entity) != entity_to_index_map.end()) {
      RemoveData(entity);
    }
  }

private:
  std::array<T, k_max_entities> component_array;
  std::unordered_map<Entity, std::size_t> entity_to_index_map;
  std::unordered_map<std::size_t, Entity> index_to_entity_map;
  std::size_t size;
};

} // namespace WE::ECS

#endif // !WE_ECS_COMPONENT_ARRAY_HPP
