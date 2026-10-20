#pragma once

#include "Graphics/GraphicsHandle.hpp"
#include <ECS/Components.hpp>

namespace Mockingbird::SDL::Graphics {

struct MEComponent(Renderable2D) {
  MeshHandle Mesh;
  MaterialHandle Material;
  TextureHandle Texture;
  PipelineHandle Pipeline;
};

} // namespace Mockingbird::SDL::Graphics
