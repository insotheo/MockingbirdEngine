#pragma once

#include "Core/InputDefinitions.hpp"
#include "Event/Event.hpp"
#include <format>

namespace Mockingbird::Core {

class ME_EVENT(KeyPressedEvent) {
public:
  KeyPressedEvent(KeyCode keycode) : m_Code(keycode) {}

  inline KeyCode GetKeyCode() const { return m_Code; }

  inline std::string ToString() const override {
    return std::format("Key pressed event({})", m_Code);
  }

private:
  KeyCode m_Code;
};

class ME_EVENT(KeyTypedEvent) {
public:
  KeyTypedEvent(char ch) : m_Ch(ch) {}

  inline char GetChar() const { return m_Ch; }

  inline std::string ToString() const override {
    return std::format("Key typed event(utf-8: {})", m_Ch);
  }

private:
  char m_Ch;
};

class ME_EVENT(KeyReleasedEvent) {
public:
  KeyReleasedEvent(KeyCode keycode) : m_Code(keycode) {}

  inline KeyCode GetKeyCode() const { return m_Code; }

  inline std::string ToString() const override {
    return std::format("Key released event({})", m_Code);
  }

private:
  KeyCode m_Code;
};

} // namespace Mockingbird::Core
