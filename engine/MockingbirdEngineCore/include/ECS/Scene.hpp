#pragma once

#include <entt/entt.hpp>

namespace Mockingbird::Core {
class Entity;

class Scene {
  friend class Entity;

public:
  Scene() = default;
  ~Scene() = default;

  Scene(const Scene &) = delete;
  Scene &operator=(const Scene &) = delete;

  Scene(Scene &&other) noexcept = default;
  Scene &operator=(Scene &&other) noexcept = default;

  Entity CreateEntity();
  void DestroyEntity(Entity entity);

  inline entt::registry &GetRegistry() { return m_Registry; }

private:
  entt::registry m_Registry;
};

} // namespace Mockingbird::Core
