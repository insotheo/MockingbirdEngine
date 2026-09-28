#include "Renderer.hpp"

#include <Core/Log.hpp>

namespace Pumpkin::SDL::Graphics {

void Renderer::Init(SDL_Window *wnd) {
  m_Wnd = wnd;

  m_Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV |
                                     SDL_GPU_SHADERFORMAT_DXIL |
                                     SDL_GPU_SHADERFORMAT_MSL,
#if PE_DEBUG
                                 true,
#else
                                 false,
#endif
                                 nullptr);

  if (!m_Device) {
    PE_LOG_CORE_ERROR("Failed to create GPU device: {}", SDL_GetError());
    m_Device = nullptr;
    return;
  }

  if (!SDL_ClaimWindowForGPUDevice(m_Device, m_Wnd)) {
    PE_LOG_CORE_ERROR("Failed to claim window for gpu: {}", SDL_GetError());
    SDL_DestroyGPUDevice(m_Device);
    m_Device = nullptr;
    return;
  }

  SetVSync(m_VSync);
}

void Renderer::Shutdown() {
  if (!m_Device)
    return;

  SDL_WaitForGPUIdle(m_Device);

  if (m_Wnd && m_Device)
    SDL_ReleaseWindowFromGPUDevice(m_Device, m_Wnd);

  SDL_DestroyGPUDevice(m_Device);

  m_Device = nullptr;
  m_Wnd = nullptr;
}

void Renderer::SetVSync(bool state) {
  if (!m_Device || !m_Wnd)
    return;

  m_VSync = state;

  SDL_GPUPresentMode presentMode =
      state ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_IMMEDIATE;

  SDL_SetGPUSwapchainParameters(m_Device, m_Wnd,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);

  PE_LOG_CORE_INFO("VSync state changed to: {}",
                   (state ? "ENABLED" : "DISABLED"));
}

void Renderer::BeginDraw2D() {
  if (m_IsDrawing)
    return;

  m_CurrentCmdBuff = SDL_AcquireGPUCommandBuffer(m_Device);
  if (!m_CurrentCmdBuff)
    return;

  // TODO framebuffer
  bool success = SDL_AcquireGPUSwapchainTexture(
      m_CurrentCmdBuff, m_Wnd, &m_CurrentTargetTexture, nullptr, nullptr);
  if (!success || !m_CurrentTargetTexture) {
    SDL_CancelGPUCommandBuffer(m_CurrentCmdBuff);
    m_CurrentCmdBuff = nullptr;
    return;
  }

  m_IsDrawing = true;
}

void Renderer::Clear(float r, float g, float b, float alpha) {
  if (!m_IsDrawing || !m_CurrentCmdBuff || m_CurrentRenderPass)
    return;

  SDL_GPUColorTargetInfo colorTargetInfo{};
  colorTargetInfo.texture = m_CurrentTargetTexture;
  colorTargetInfo.clear_color = SDL_FColor{r, g, b, alpha};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

  m_CurrentRenderPass =
      SDL_BeginGPURenderPass(m_CurrentCmdBuff, &colorTargetInfo, 1, nullptr);
}

void Renderer::EndDraw2D() {
  if (!m_IsDrawing)
    return;

  if (m_CurrentRenderPass) {
    SDL_EndGPURenderPass(m_CurrentRenderPass);
    m_CurrentRenderPass = nullptr;
  }

  if (m_CurrentCmdBuff) {
    SDL_SubmitGPUCommandBuffer(m_CurrentCmdBuff);
    m_CurrentCmdBuff = nullptr;
  }

  m_CurrentTargetTexture = nullptr;
  m_IsDrawing = false;
}

} // namespace Pumpkin::SDL::Graphics
