#pragma once

#include "Core/Math.hpp"

#define MEComponent(name) ME##name##Component

namespace Mockingbird::Core {

struct MEComponent(Transform) {
  MEVec3 Position{0.f};
  MEVec3 Scale{1.f};
  MEVec3 Rotation{0.f};
};

} // namespace Mockingbird::Core
