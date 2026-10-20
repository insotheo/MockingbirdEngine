#pragma once

#include "ECS/Entity.hpp"
#include "ECS/Scene.hpp"

namespace Mockingbird::Core {

struct SceneHandle {
  Scene *RawScene = nullptr;

  explicit operator bool() const { return RawScene != nullptr; }
  bool IsValid() const { return RawScene != nullptr; }

  inline Entity CreateEntity() {
    return RawScene ? RawScene->CreateEntity() : Entity{};
  }
  void DestroyEntity(Entity entity) {
    if (RawScene)
      RawScene->DestroyEntity(entity);
  }

  Scene *operator->() { return RawScene; }
  const Scene *operator->() const { return RawScene; }
};

} // namespace Mockingbird::Core
