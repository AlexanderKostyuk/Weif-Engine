#include "ECS/entity_manager.h"

namespace WE::ECS {

EntityManager::EntityManager() {
  for (Entity entity = 0; entity < k_max_entities; ++entity)
    available_entities.push(entity);
}

Entity EntityManager::CreateEntity() {
  Entity entity = available_entities.front();
  available_entities.pop();
  available_entities_count++;
  return entity;
}

void EntityManager::DestroyEntity(Entity entity) {
  signatures[entity].reset();

  available_entities.push(entity);
  available_entities_count--;
}

} // namespace WE::ECS
