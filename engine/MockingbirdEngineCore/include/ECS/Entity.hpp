#pragma once

#include "ECS/Scene.hpp"
#include <entt/entt.hpp>
#include <utility>

namespace Mockingbird::Core {

using EntityHandle = entt::entity;

class Entity {
public:
  Entity() = default;
  Entity(EntityHandle handle, Scene *scene)
      : m_EntityHnd(handle), m_Scene(scene) {}

  template <typename T, typename... Args> T &AddComponent(Args &&...args) {
    return m_Scene->m_Registry.emplace<T>(m_EntityHnd,
                                          std::forward<Args>(args)...);
  }

  template <typename T> T &GetComponent() {
    return m_Scene->m_Registry.get<T>(m_EntityHnd);
  }

  template <typename T> void RemoveComponent() {
    m_Scene->m_Registry.remove<T>(m_EntityHnd);
  }

  operator EntityHandle() const { return m_EntityHnd; }

private:
  EntityHandle m_EntityHnd{entt::null};
  Scene *m_Scene = nullptr;
};

// inline methods for Scene. (see Core/Scene.hpp)
inline Entity Scene::CreateEntity() {
  return Entity(m_Registry.create(), this);
}

inline void Scene::DestroyEntity(Entity entity) { m_Registry.destroy(entity); }

} // namespace Mockingbird::Core
