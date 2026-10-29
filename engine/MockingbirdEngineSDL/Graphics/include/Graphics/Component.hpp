#pragma once

#include "Graphics/Camera.hpp"
#include "Graphics/GraphicsHandle.hpp"
#include <ECS/Components.hpp>

namespace Mockingbird::SDL::Graphics {

struct MEComponent(Renderable) {
  MeshHandle Mesh;
  PipelineHandle Pipeline;
  MaterialHandle Material;
};

struct MEComponent(Camera) {
  Camera CamInstance;
  Core::MEVec4 ClearColor{0.1f, 0.1f, 0.1f, 1.0f};
  SDL_GPUTexture *TargetTexture;
  int RenderOrder = 0;
  bool IsActive = true;
};

} // namespace Mockingbird::SDL::Graphics
