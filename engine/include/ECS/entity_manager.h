#ifndef WE_ECS_ENTITY_MANAGER_H
#define WE_ECS_ENTITY_MANAGER_H

#include <array>
#include <bitset>
#include <cstdint>
#include <deque>
namespace WE::ECS {

using Entity = std::uint32_t;
const Entity k_max_entities = 500000;

using ComponentType = std::uint8_t;
const ComponentType k_max_components = 128;

using Signature = std::bitset<k_max_components + 1>;
// Default signature, last bit defines entity existance
const Signature k_default_signature = Signature().set(k_max_components);

class EntityManager {
private:
  void FindLastEntity();

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

  inline Entity GetLastEntity() const { return last_entity; }

private:
  std::deque<Entity> available_entities{};
  std::array<Signature, k_max_entities> signatures{};
  Entity last_entity = 0;
};

} // namespace WE::ECS

#endif // !WE_ENTITY_MANAGER_H
