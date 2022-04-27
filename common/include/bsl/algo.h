#pragma once

#include <config.h>

namespace bsl {

inline constexpr bool collide(uint64_t addr1, uint64_t size1, uint64_t addr2,
                       uint64_t size2) noexcept {
  return !(addr1 + size1 <= addr2 || addr2 + size2 <= addr1);
}

inline constexpr bool has_attr(uint64_t attr, uint64_t chk) {
  return (attr & chk) == chk;
}

}  // namespace bsl