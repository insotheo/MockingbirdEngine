#pragma once

#include <Core/Math.hpp>
#include <ECS/Components.hpp>
#include <SDL3/SDL.h>

namespace Mockingbird::SDL::Graphics {
class Camera {
public:
  enum class Mode { Perspective, Orthographic };

  Camera(Mode mode = Mode::Orthographic) : m_Mode(mode) { UpdateMatrices(); }

  inline void SetPosition(const Core::MEVec3 &pos) {
    m_Pos = pos;
    m_Dirty = true;
  }
  inline void Move(const Core::MEVec3 &delta) {
    m_Pos += delta;
    m_Dirty = true;
  }

  inline void SetRotation(const Core::MEVec3 &rot) {
    m_Rot = rot;
    m_Dirty = true;
  }
  inline void Rotate(const Core::MEVec3 &delta) {
    m_Rot += delta;
    m_Dirty = true;
  }

  inline void SetMode(Mode mode) {
    m_Mode = mode;
    m_Dirty = true;
  }

  inline void SetPerspective(float fovDeg, float aspectRatio, float nearClip,
                             float farClip) {
    m_FOV = ME_DEG_TO_RAD(fovDeg);
    m_AspectRatio = aspectRatio;
    m_NearClip = nearClip;
    m_FarClip = farClip;
    m_Mode = Mode::Perspective;
    m_Dirty = true;
  }

  void SetOrthographic(float left, float right, float bottom, float top,
                       float nearClip = -1.f, float farClip = 1.f) {
    m_OrthoLeft = left;
    m_OrthoRight = right;
    m_OrthoBottom = bottom;
    m_OrthoTop = top;
    m_NearClip = nearClip;
    m_FarClip = farClip;
    m_Mode = Mode::Orthographic;
    m_Dirty = true;
  }

  inline const Core::MEMat4 &GetViewMatrix() {
    if (m_Dirty)
      UpdateMatrices();
    return m_ViewMat;
  }

  inline const Core::MEMat4 &GetProjectionMatrix() {
    if (m_Dirty)
      UpdateMatrices();
    return m_ProjMat;
  }

  inline const Core::MEMat4 GetViewProjectionMatrix() {
    if (m_Dirty)
      UpdateMatrices();
    return m_ProjMat * m_ViewMat;
  }

private:
  void UpdateMatrices();

  Mode m_Mode;
  bool m_Dirty = false;

  Core::MEVec3 m_Pos{0.f};
  Core::MEVec3 m_Rot{0.f};

  float m_FOV = ME_DEG_TO_RAD(60.f);
  float m_AspectRatio = 16.f / 9.f;

  float m_OrthoLeft = -1.0f;
  float m_OrthoRight = 1.0f;
  float m_OrthoBottom = -1.0f;
  float m_OrthoTop = 1.0f;

  float m_NearClip = 0.001f;
  float m_FarClip = 1000.0f;

  Core::MEMat4 m_ViewMat{1.f};
  Core::MEMat4 m_ProjMat{1.f};
};
} // namespace Mockingbird::SDL::Graphics
