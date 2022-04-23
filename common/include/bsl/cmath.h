#pragma once

#include <config.h>

namespace bsl {

/**
 * @brief log2(x), x must be power of 2
 */
inline constexpr int p2log2(uint64_t x) { return __builtin_ctzll(x); }

}  // namespace bsl