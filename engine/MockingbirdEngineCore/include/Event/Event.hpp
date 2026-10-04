#pragma once

#include <cstdint>
#include <string>

namespace Mockingbird::Core {

class Event {
public:
  virtual ~Event() = default;

  virtual std::string ToString() const = 0;
  virtual uint32_t GetTypeID() const = 0;

  bool Handled = false;
  const void *NativeEvent = nullptr;
};

class EventTypeCounter {
public:
  static uint32_t GetNextID();
};

template <typename T> class EventImpl : public Event {
public:
  static uint32_t GetStaticTypeID() {
    static uint32_t s_TypeID = EventTypeCounter::GetNextID();
    return s_TypeID;
  }

  inline uint32_t GetTypeID() const override { return GetStaticTypeID(); }
};

#define ME_EVENT(eventName)                                                    \
  eventName:                                                                   \
public                                                                         \
  Mockingbird::Core::EventImpl<eventName>

class EventDispatcher {
public:
  EventDispatcher(Event &event) : m_Event(event) {}

  template <typename T, typename Func> void Dispatch(Func &&fn) {
    if (m_Event.Handled)
      return;

    if (m_Event.GetTypeID() == T::GetStaticTypeID()) {
      fn(static_cast<T &>(m_Event));
    }
  }

private:
  Event &m_Event;
};

} // namespace Mockingbird::Core
