#pragma once

#include <cstdlib>
#include <iostream>

#define PE_DEBUGBREAK() std::abort()

#ifdef PE_DEBUG
#define PE_ASSERT(condition, msg)                                              \
  do {                                                                         \
    if (!(condition)) {                                                        \
      std::cerr << "========================================\n"                \
                << "[PUMPKIN ENGINE ASSERT FAILED]\n"                          \
                << "Message: " << msg << "\n"                                  \
                << "File: " << __FILE__ << "\n"                                \
                << "Line: " << __LINE__ << "\n"                                \
                << "========================================\n";               \
      PE_DEBUGBREAK();                                                         \
    }                                                                          \
  } while (0);
#else
#define PE_ASSERT(condition, msg)
#endif
