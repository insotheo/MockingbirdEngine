#pragma once

#include <cstdlib>
#include <iostream>

#define ME_DEBUGBREAK() std::abort()

#ifdef ME_DEBUG
#define ME_ASSERT(condition, msg)                                              \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "========================================\n"                \
                << "[PUMPKIN ENGINE ASSERT FAILED]\n"                          \
                << "Message: " << msg << "\n"                                  \
                << "File: " << __FILE__ << "\n"                                \
                << "Line: " << __LINE__ << "\n"                                \
                << "========================================\n";               \
      ME_DEBUGBREAK();                                                         \
    }                                                                          \
  } while (0);
#else
#define ME_ASSERT(condition, msg)
#endif
