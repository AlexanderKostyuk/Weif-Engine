#include "ECS/entity_manager.h"

#include <stack>

namespace WE::ECS {

EntityManager::EntityManager() {
  for (Entity entity = 0; entity < k_max_entities; ++entity)
    available_entities.push(entity);
}

Entity EntityManager::CreateEntity() {
  Entity entity = available_entities.top();
  available_entities.pop();
  max_entity = std::max(max_entity, entity);
  return entity;
}

void EntityManager::DestroyEntity(Entity entity) {
  signatures[entity].reset();
  available_entities.push(entity);
  if (max_entity <= entity)
    max_entity--;
}

} // namespace WE::ECS
