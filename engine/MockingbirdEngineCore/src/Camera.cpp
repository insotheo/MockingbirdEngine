#include "Core/Camera.hpp"

namespace Mockingbird::Core {

void Camera::UpdateMatrices() {
  MEMat4 trform = Math::Translate(MEMat4(1.f), m_Pos);

  trform = Math::Rotate(trform, ME_DEG_TO_RAD(m_Rot.x), MEVec3(1.0f, 0.f, 0.f));
  trform = Math::Rotate(trform, ME_DEG_TO_RAD(m_Rot.y), MEVec3(0.0f, 1.f, 0.f));
  trform = Math::Rotate(trform, ME_DEG_TO_RAD(m_Rot.z), MEVec3(0.0f, 0.f, 1.f));

  m_ViewMat = Math::Inverse(trform);

  if (m_Mode == Mode::Orthographic) {
    m_ProjMat = Math::Ortho(m_OrthoLeft, m_OrthoRight, m_OrthoBottom,
                            m_OrthoTop, m_NearClip, m_FarClip);
  } else {
    m_ProjMat = Math::Perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
  }

  m_Dirty = false;
}

} // namespace Mockingbird::Core
