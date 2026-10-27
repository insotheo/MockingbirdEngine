#pragma once

#include "glm/ext/matrix_clip_space.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define ME_DEG_TO_RAD(deg) (glm::radians(deg))
#define ME_RAD_TO_DEG(rad) (glm::degrees(deg))

namespace Mockingbird::Core {

using MEVec2 = glm::vec2;
using MEVec2i = glm::ivec2;
using MEVec3 = glm::vec3;
using MEVec3i = glm::ivec3;
using MEVec4 = glm::vec4;
using MEVec4i = glm::ivec4;
using MEMat3 = glm::mat3;
using MEMat4 = glm::mat4;

namespace Math {

inline MEMat4 Ortho(float left, float right, float bottom, float top,
                    float zNear, float zFar) {
  return glm::ortho(left, right, bottom, top, zNear, zFar);
}

inline MEMat4 Perspective(float fov, float aspect, float zNear, float zFar) {
  return glm::perspective(fov, aspect, zNear, zFar);
}

inline MEMat4 Inverse(const MEMat4 &mat) { return glm::inverse(mat); }

inline MEMat4 Translate(const MEMat4 &m, const MEVec3 &v) {
  return glm::translate(m, v);
}

inline MEMat4 Scale(const MEMat4 &m, const MEVec3 &v) {
  return glm::scale(m, v);
}

inline MEMat4 Rotate(const MEMat4 &m, float angle, const MEVec3 &v) {
  return glm::rotate(m, angle, v);
}

inline MEMat4 Model(const MEVec3 &pos, const MEVec3 &scale, const MEVec3 &rot) {
  MEMat4 model{1.f};
  model = Translate(model, pos);
  model = Rotate(model, ME_DEG_TO_RAD(rot.x), MEVec3(1.f, 0.f, 0.f));
  model = Rotate(model, ME_DEG_TO_RAD(rot.y), MEVec3(0.f, 1.f, 0.f));
  model = Rotate(model, ME_DEG_TO_RAD(rot.z), MEVec3(0.f, 0.f, 1.f));
  model = Scale(model, scale);
  return model;
}

inline const float *ValuePtrMat3(const MEMat3 &m) { return glm::value_ptr(m); }
inline const float *ValuePtrMat4(const MEMat4 &m) { return glm::value_ptr(m); }

} // namespace Math

} // namespace Mockingbird::Core
