#pragma once

#include <bsl/bswap.h>
#include <bsl/config.h>

#include <concepts>
#include <cstdint>

namespace bsl {

// big endian types
// template <typename T>
// requires std::integral<T>
template <std::integral T>
class big_endian {
  T value;

 public:
  constexpr big_endian(T value) noexcept : value(value) {}
  FORCE_INLINE constexpr operator T() const noexcept { return bswap(value); }
};

}  // namespace bsl

using bu16_t = bsl::big_endian<uint16_t>;
using bu32_t = bsl::big_endian<uint32_t>;
using bu64_t = bsl::big_endian<uint64_t>;
using bi16_t = bsl::big_endian<int16_t>;
using bi32_t = bsl::big_endian<int32_t>;
using bi64_t = bsl::big_endian<int64_t>;