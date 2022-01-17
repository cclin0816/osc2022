#pragma once

#include <bsl/bswap.h>

#include <concepts>
#include <cstdint>

namespace bsl {

// big endian types
template <typename T>
requires std::integral<T>
class be_t {
  T value;

 public:
  constexpr be_t(T value) : value(value) {}
  inline constexpr operator T() const { return bswap(value); }
};

}  // namespace bsl

using bu16_t = bsl::be_t<uint16_t>;
using bu32_t = bsl::be_t<uint32_t>;
using bu64_t = bsl::be_t<uint64_t>;
using bi16_t = bsl::be_t<int16_t>;
using bi32_t = bsl::be_t<int32_t>;
using bi64_t = bsl::be_t<int64_t>;