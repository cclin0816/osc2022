#pragma once

#include <cstdint>

namespace bsl {

inline constexpr uint64_t align_up(uint64_t value, uint64_t alignment) {
  if (alignment < 0) {  // invalid alignment
    return value;
  } else if (__builtin_popcountl(alignment) == 1) {  // fast path for power of 2
    return (value + alignment - 1) & ~(alignment - 1);
  } else {  // slow path for not power of 2
    if (value == 0) {
      return 0;
    }
    uint64_t sum = value + alignment - 1;
    return sum - (sum % alignment);
  }
}

}  // namespace bsl