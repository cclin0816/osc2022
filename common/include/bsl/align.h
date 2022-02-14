#pragma once

#include <bsl/config.h>

#include <cstddef>

namespace bsl {

template <typename T>
FORCE_INLINE constexpr T p2align_up(T value, size_t alignment) noexcept {
  return __builtin_align_up(value, alignment);
}

template <typename T>
FORCE_INLINE constexpr T p2align_down(T value, size_t alignment) noexcept {
  return __builtin_align_down(value, alignment);
}

template <typename T>
FORCE_INLINE constexpr bool is_p2aligned(T value, size_t alignment) noexcept {
  return __builtin_is_aligned(value, alignment);
}

}  // namespace bsl