#pragma once

#include <format>
#include <iostream>

#ifdef ME_DEBUG

// DEFAULT
#define ME_LOG_TRACE(...)                                                      \
  do {                                                                         \
    std::cout << "\e[0m[TRACE]: " << std::format(__VA_ARGS__) << "\e[0m\n";    \
  } while (0)

#define ME_LOG_INFO(...)                                                       \
  do {                                                                         \
    std::cout << "\e[0;32m[INFO]: " << std::format(__VA_ARGS__) << "\e[0m\n";  \
  } while (0)

#define ME_LOG_WARN(...)                                                       \
  do {                                                                         \
    std::cout << "\e[0;33m[WARN]: " << std::format(__VA_ARGS__) << "\e[0m\n";  \
  } while (0)

#define ME_LOG_ERROR(...)                                                      \
  do {                                                                         \
    std::cerr << "\e[0;31m[ERROR]: " << std::format(__VA_ARGS__) << "\e[0m\n"; \
  } while (0)

// CORE
#define ME_LOG_CORE_TRACE(...)                                                 \
  do {                                                                         \
    std::cout << "\e[0m[TRACE(CORE)]: " << std::format(__VA_ARGS__)            \
              << "\e[0m\n";                                                    \
  } while (0)
#define ME_LOG_CORE_INFO(...)                                                  \
  do {                                                                         \
    std::cout << "\e[1;32m[INFO(CORE)]: " << std::format(__VA_ARGS__)          \
              << "\e[0m\n";                                                    \
  } while (0)
#define ME_LOG_CORE_WARN(...)                                                  \
  do {                                                                         \
    std::cout << "\e[1;33m[WARN(CORE)]: " << std::format(__VA_ARGS__)          \
              << "\e[0m\n";                                                    \
  } while (0)
#define ME_LOG_CORE_ERROR(...)                                                 \
  do {                                                                         \
    std::cerr << "\e[1;31m[ERROR(CORE)]: " << std::format(__VA_ARGS__)         \
              << "\e[0m\n";                                                    \
  } while (0)

#else

#define ME_LOG_TRACE(...)
#define ME_LOG_INFO(...)
#define ME_LOG_WARN(...)
#define ME_LOG_ERROR(...)

#define ME_LOG_CORE_TRACE(...)
#define ME_LOG_CORE_INFO(...)
#define ME_LOG_CORE_WARN(...)
#define ME_LOG_CORE_ERROR(...)

#endif
