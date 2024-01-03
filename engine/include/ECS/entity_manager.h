#ifndef WE_ECS_ENTITY_MANAGER_H
#define WE_ECS_ENTITY_MANAGER_H

#include <array>
#include <bitset>
#include <cstdint>
#include <queue>
namespace WE::ECS {

using Entity = std::uint32_t;
const Entity k_max_entities = 500000;

using ComponentType = std::uint8_t;
const ComponentType k_max_components = 128;

using Signature = std::bitset<k_max_components>;

class EntityManager {
public:
  EntityManager();

  Entity CreateEntity();
  void DestroyEntity(Entity entity);

  inline void SetSignature(Entity entity, Signature signature) {
    signatures[entity] = signature;
  }

  inline Signature GetSignature(Entity entity) const {
    return signatures.at(entity);
  }

private:
  std::queue<Entity> available_entities{};
  std::array<Signature, k_max_entities> signatures{};
  std::uint32_t available_entities_count;
};

} // namespace WE::ECS

#endif // !WE_ENTITY_MANAGER_H
