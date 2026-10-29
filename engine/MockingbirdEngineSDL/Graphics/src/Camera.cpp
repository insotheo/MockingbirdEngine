#include "Graphics/Camera.hpp"

namespace Mockingbird::SDL::Graphics {

void Camera::UpdateMatrices() {
  Core::MEMat4 trform = Core::Math::Translate(Core::MEMat4(1.f), m_Pos);

  trform = Core::Math::Rotate(trform, ME_DEG_TO_RAD(m_Rot.x),
                              Core::MEVec3(1.0f, 0.f, 0.f));
  trform = Core::Math::Rotate(trform, ME_DEG_TO_RAD(m_Rot.y),
                              Core::MEVec3(0.0f, 1.f, 0.f));
  trform = Core::Math::Rotate(trform, ME_DEG_TO_RAD(m_Rot.z),
                              Core::MEVec3(0.0f, 0.f, 1.f));

  m_ViewMat = Core::Math::Inverse(trform);

  if (m_Mode == Mode::Orthographic) {
    m_ProjMat = Core::Math::Ortho(m_OrthoLeft, m_OrthoRight, m_OrthoBottom,
                                  m_OrthoTop, m_NearClip, m_FarClip);
  } else {
    m_ProjMat =
        Core::Math::Perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
  }

  m_Dirty = false;
}

} // namespace Mockingbird::SDL::Graphics
