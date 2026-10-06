#pragma once

#include <format>
#include <glm/glm.hpp>

// MATH TYPES FORMATING FOR std::format
namespace std {

// FOR MEVec2
template <> struct formatter<glm::vec2> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::vec2 &v, format_context &ctx) const {
    return format_to(ctx.out(), "MEVec2({}, {})", v.x, v.y);
  }
};

// FOR MEVec2i
template <> struct formatter<glm::ivec2> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::ivec2 &v, format_context &ctx) const {
    return format_to(ctx.out(), "MEVec2i({}, {})", v.x, v.y);
  }
};

// FOR MEVec3
template <> struct formatter<glm::vec3> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::vec3 &v, format_context &ctx) const {
    return format_to(ctx.out(), "MEVec3({}, {})", v.x, v.y, v.z);
  }
};

// FOR MEVec3i
template <> struct formatter<glm::ivec3> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::ivec3 &v, format_context &ctx) const {
    return format_to(ctx.out(), "MEVec3i({}, {})", v.x, v.y, v.z);
  }
};

// FOR MEVec4
template <> struct formatter<glm::vec4> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::vec4 &v, format_context &ctx) const {
    return format_to(ctx.out(), "MEVec4({}, {})", v.x, v.y, v.z, v.w);
  }
};

// FOR MEVec4i
template <> struct formatter<glm::ivec4> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::ivec4 &v, format_context &ctx) const {
    return format_to(ctx.out(), "MEVec4i({}, {})", v.x, v.y, v.z, v.w);
  }
};

// FOR MEMat3
template <> struct formatter<glm::mat3> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::mat3 &m, format_context &ctx) const {
    return format_to(ctx.out(),
                     "MEMat3(\n\t[{}, {}, {}]\n\t[{}, {}, {}]\n\t[{}, "
                     "{}, {}]\n)",

                     // clang-format off
                     m[0][0], m[1][0], m[2][0],
                     m[0][1], m[1][1], m[2][1],
                     m[0][2], m[1][2], m[2][2]
                    );
    // clang-format on
  }
};

// FOR MEMat4
template <> struct formatter<glm::mat4> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  auto format(const glm::mat4 &m, format_context &ctx) const {
    return format_to(ctx.out(),
                     "MEMat4(\n\t[{}, {}, {}, {}]\n\t[{}, {}, {}, {}]\n\t[{}, "
                     "{}, {}, {}]\n\t[{}, {}, {}, {}]\n)",

                     // clang-format off
                     m[0][0], m[1][0], m[2][0], m[3][0],
                     m[0][1], m[1][1], m[2][1], m[3][1],
                     m[0][2], m[1][2], m[2][2], m[3][2],
                     m[0][3], m[1][3], m[2][3], m[3][3]
                    );
    // clang-format on
  }
};

} // namespace std
