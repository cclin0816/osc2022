#include <config.h>

#include <charconv>
#include <concepts>

namespace bsl {

template <typename T>
size_t to_chars(char *buf, size_t buf_len, T val, uint32_t base = 10) {
  auto [ptr, ec] = std::to_chars(buf, buf + buf_len, val, base);
  return ptr - buf;
}

}  // namespace bsl
