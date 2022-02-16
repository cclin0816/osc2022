#include <config.h>

extern "C" {

[[noreturn]] void abort();

void abort_msg(const char* msg, const char* file, unsigned line,
               const char* func);

}

[[noreturn]] FORCE_INLINE void __assert_fail(const char* msg, const char* file,
                                unsigned line, const char* func) {
  abort_msg(msg, file, line, func);
  abort();
}

#define assert(expr)                                                 \
  do {                                                               \
    if (!static_cast<bool>(expr)) [[unlikely]]                       \
      __assert_fail(#expr, __FILE__, __LINE__, __PRETTY_FUNCTION__); \
  } while (0)

namespace bsl {

#define panic() __assert_fail("panic", __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define unreachable() \
  __assert_fail("unreachable", __FILE__, __LINE__, __PRETTY_FUNCTION__)

#define todo() __assert_fail("todo", __FILE__, __LINE__, __PRETTY_FUNCTION__)

}  // namespace bsl