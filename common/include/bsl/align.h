#pragma once

#include <cstddef>

namespace bsl {

// alignment not positive power of 2 is undefined behavior
template <typename T>
inline constexpr T align_up(T value, size_t alignment) {
  return __builtin_align_up(value, alignment);
}

// alignment not positive power of 2 is undefined behavior
template <typename T>
inline constexpr T align_down(T value, size_t alignment) {
  return __builtin_align_down(value, alignment);
}

// alignment not positive power of 2 is undefined behavior
template <typename T>
inline constexpr bool is_aligned(T value, size_t alignment) {
  return __builtin_is_aligned(value, alignment);
}

}  // namespace bsl