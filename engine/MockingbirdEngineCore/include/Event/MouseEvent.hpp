#pragma once

#include "Event/Event.hpp"
#include <format>

namespace Mockingbird::Core {

class ME_EVENT(MouseMovedEvent) {
public:
  MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

  inline float GetX() const { return m_X; }
  inline float GetY() const { return m_Y; }

  inline std::string ToString() const override {
    return std::format("Mouse moved event({};{})", m_X, m_Y);
  }

private:
  float m_X, m_Y;
};

class ME_EVENT(MouseButtonPressedEvent) {
public:
  MouseButtonPressedEvent(uint8_t button) : m_Button(button) {}

  inline uint8_t GetButton() const { return m_Button; }

  inline std::string ToString() const override {
    return std::format("Mouse button pressed event({})", m_Button);
  }

private:
  uint8_t m_Button;
};

class ME_EVENT(MouseButtonReleasedEvent) {
public:
  MouseButtonReleasedEvent(uint8_t button) : m_Button(button) {}

  inline uint8_t GetButton() const { return m_Button; }

  inline std::string ToString() const override {
    return std::format("Mouse button released event({})", m_Button);
  }

private:
  uint8_t m_Button;
};

class ME_EVENT(MouseScrollEvent) {
public:
  MouseScrollEvent(float xOff, float yOff) : m_XOff(xOff), m_YOff(yOff) {}

  inline float GetXOffset() const { return m_XOff; }
  inline float GetYOffset() const { return m_YOff; }

  inline std::string ToString() const override {
    return std::format("Mouse scroll event({};{})", m_XOff, m_YOff);
  }

private:
  float m_XOff, m_YOff;
};

} // namespace Mockingbird::Core
