#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Mockingbird::Core {

using MEVec2 = glm::vec2;
using MEVec3 = glm::vec3;
using MEVec4 = glm::vec4;
using MEMat4 = glm::mat4;

namespace Math {

inline MEMat4 Ortho(float left, float right, float bottom, float top,
                    float zNear, float zFar) {
  return glm::ortho(left, right, bottom, top, zNear, zFar);
}

inline MEMat4 Translate(const MEMat4 &m, const MEVec3 &v) {
  return glm::translate(m, v);
}

inline MEMat4 Scale(const MEMat4 &m, const MEVec3 &v) {
  return glm::scale(m, v);
}

inline const float *ValuePtr(const MEMat4 &m) { return glm::value_ptr(m); }

} // namespace Math

} // namespace Mockingbird::Core
