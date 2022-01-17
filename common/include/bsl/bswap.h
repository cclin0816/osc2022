#pragma once

#include <concepts>
#include <cstddef>

namespace bsl {

// byte swap 16, 32 64 bit values
template <typename T>
requires std::integral<T>
inline constexpr T bswap(T x) {
  constexpr size_t len = sizeof(T);
  static_assert(len == 2 || len == 4 || len == 8, "invalid size");
  if (len == 2) {
    return __builtin_bswap16(x);
  } else if (len == 4) {
    return __builtin_bswap32(x);
  } else {
    return __builtin_bswap64(x);
  }
}

}  // namespace bsl