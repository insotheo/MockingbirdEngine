#pragma once

#include <SDL3/SDL.h>
#include <cstdint>
#include <string>

namespace Mockingbird::SDL::Graphics {

struct ShaderStageDesc {
  uint32_t SamplerCount = 0;
  uint32_t StorageBufferCount = 0;
  uint32_t StorageTextureCount = 0;
  uint32_t UniformBufferCount = 0;
  std::string EntryPoint = "main";
};

struct ShaderProgramDesc {
  ShaderStageDesc Vertex{};
  ShaderStageDesc Fragment{};
};

struct PipelineStates {
  SDL_GPUPrimitiveType PrimitiveType = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  SDL_GPURasterizerState RasterizerState{
      .fill_mode = SDL_GPU_FILLMODE_FILL,
      .cull_mode = SDL_GPU_CULLMODE_BACK,
      .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
  };
  SDL_GPUDepthStencilState DepthStencilState{
      .compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
      .enable_depth_test = true,
      .enable_depth_write = true,
  };

  SDL_GPUTextureFormat ColorTargetFormat = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
  SDL_GPUTextureFormat DepthStencilFormat =
      SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
};

} // namespace Mockingbird::SDL::Graphics
