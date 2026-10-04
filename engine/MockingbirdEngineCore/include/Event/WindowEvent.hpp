#pragma once

#include "Event/Event.hpp"
#include <format>

namespace Mockingbird::Core {

class ME_EVENT(WindowCloseEvent) {
public:
  inline std::string ToString() const override { return "Window close event"; }
};

class ME_EVENT(WindowResizeEvent) {
public:
  WindowResizeEvent(uint32_t widht, uint32_t height)
      : m_Width(widht), m_Height(height) {}

  inline uint32_t GetWidth() const { return m_Width; }
  inline uint32_t GetHeight() const { return m_Height; }

  inline std::string ToString() const override {
    return std::format("Window resize event({};{})", m_Width, m_Height);
  }

private:
  uint32_t m_Width, m_Height;
};

class ME_EVENT(WindowMoveEvent) {
public:
  WindowMoveEvent(uint32_t x, uint32_t y) : m_X(x), m_Y(y) {}

  inline uint32_t GetX() const { return m_X; }
  inline uint32_t GetY() const { return m_Y; }

  inline std::string ToString() const override {
    return std::format("Window move event({};{})", m_X, m_Y);
  }

private:
  uint32_t m_X, m_Y;
};

} // namespace Mockingbird::Core
