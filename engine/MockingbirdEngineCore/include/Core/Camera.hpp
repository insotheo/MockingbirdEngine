#pragma once

#include "Core/Math.hpp"

namespace Mockingbird::Core {
class Camera {
public:
  enum class Mode { Perspective, Orthographic };

  Camera(Mode mode = Mode::Orthographic) : m_Mode(mode) { UpdateMatrices(); }

  inline void SetPosition(const MEVec3 &pos) {
    m_Pos = pos;
    m_Dirty = true;
  }
  inline void Move(const MEVec3 &delta) {
    m_Pos += delta;
    m_Dirty = true;
  }

  inline void SetRotation(const MEVec3 &rot) {
    m_Rot = rot;
    m_Dirty = true;
  }
  inline void Rotate(const MEVec3 &delta) {
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

  inline const MEMat4 &GetViewMatrix() {
    if (m_Dirty)
      UpdateMatrices();
    return m_ViewMat;
  }

  inline const MEMat4 &GetProjectionMatrix() {
    if (m_Dirty)
      UpdateMatrices();
    return m_ProjMat;
  }

  inline const MEMat4 GetViewProjectionMatrix() {
    if (m_Dirty)
      UpdateMatrices();
    return m_ProjMat * m_ViewMat;
  }

private:
  void UpdateMatrices();

  Mode m_Mode;
  bool m_Dirty = false;

  MEVec3 m_Pos{0.f};
  MEVec3 m_Rot{0.f};

  float m_FOV = ME_DEG_TO_RAD(60.f);
  float m_AspectRatio = 16.f / 9.f;

  float m_OrthoLeft = -1.0f;
  float m_OrthoRight = 1.0f;
  float m_OrthoBottom = -1.0f;
  float m_OrthoTop = 1.0f;

  float m_NearClip = 0.001f;
  float m_FarClip = 1000.0f;

  MEMat4 m_ViewMat{1.f};
  MEMat4 m_ProjMat{1.f};
};
} // namespace Mockingbird::Core
