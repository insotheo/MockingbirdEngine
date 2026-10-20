#pragma once

#include "Core/Math.hpp"

#define MEComponent(name) ME##name##Component

namespace Mockingbird::Core {

struct MEComponent(Transform2D) {
  MEVec2 Position;
  MEVec2 Scale;
  float Rotation;
};

} // namespace Mockingbird::Core
