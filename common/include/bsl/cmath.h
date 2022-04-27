#pragma once

#include <config.h>

#include <bit>

namespace bsl {

inline constexpr int log2_floor(auto num) {
  return std::countr_zero(std::bit_floor(num));
}

inline constexpr int log2_ceil(auto num) {
  ASSUME(num > 1UL);
  return std::countr_zero(std::bit_ceil(num));
}

}  // namespace bsl