#include "ECS/entity_manager.h"

#include <deque>

namespace WE::ECS {

EntityManager::EntityManager() {
  for (Entity entity = 0; entity < k_max_entities; ++entity)
    available_entities.push_back(entity);
}

void EntityManager::FindLastEntity() {
  while ((signatures[last_entity] & k_default_signature) != k_default_signature)
    last_entity--;
}

Entity EntityManager::CreateEntity() {
  Entity entity = available_entities.front();
  available_entities.pop_front();
  signatures[entity] = k_default_signature;
  last_entity = std::max(last_entity, entity);
  return entity;
}

void EntityManager::DestroyEntity(Entity entity) {
  if ((signatures[entity] & k_default_signature) != k_default_signature)
    return;
  signatures[entity].reset();

  available_entities.push_front(entity);
  FindLastEntity();
}

} // namespace WE::ECS
