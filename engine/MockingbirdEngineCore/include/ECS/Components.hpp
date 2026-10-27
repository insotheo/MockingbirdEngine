#pragma once

#include "Core/Camera.hpp"
#include "Core/Math.hpp"

#define MEComponent(name) ME##name##Component

namespace Mockingbird::Core {

struct MEComponent(Transform) {
  MEVec3 Position{0.f};
  MEVec3 Scale{1.f};
  MEVec3 Rotation{0.f};
};

struct MEComponent(Camera) {
  Camera CamInstance;
  MEVec4 ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
  void *TargetTexture;
  int RenderOrder = 0;
  bool IsActive = true;
};

} // namespace Mockingbird::Core
