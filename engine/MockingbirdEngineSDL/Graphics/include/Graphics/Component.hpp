#pragma once

#include "Graphics/GraphicsHandle.hpp"
#include <ECS/Components.hpp>

namespace Mockingbird::SDL::Graphics {

struct MEComponent(Renderable) {
  MeshHandle Mesh;
  PipelineHandle Pipeline;
  MaterialHandle Material;
};

} // namespace Mockingbird::SDL::Graphics
