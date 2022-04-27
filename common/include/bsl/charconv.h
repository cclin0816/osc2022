#pragma once

#include <config.h>

#include <charconv>
#include <concepts>
#include <system_error>
#include <bsl/cctype.h>

namespace bsl {

template <typename T>
NOINLINE size_t to_chars(char *buf, size_t buf_len, T val, int32_t base = 10) {
  auto [ptr, ec] = std::to_chars(buf, buf + buf_len, val, base);
  if (ec != std::errc{}) {
    return 0;
  }
  return (size_t)(ptr - buf);
}

// template <typename T>
NOINLINE size_t from_chars(const char *buf, size_t buf_len, uint64_t &val,
                           int32_t base = 10) {
  auto buf_itr = buf;
  auto buf_end = buf + buf_len;
  val = 0;
  for(; buf_itr != buf_end; ++buf_itr) {
    auto digit = bsl::to_digit(*buf_itr);
    if(digit < 0 || digit >= base) {
      return (size_t)(buf_itr - buf);
    }
    val = (val * (uint64_t)base) + (uint64_t)digit;
  }
  return (size_t)(buf_itr - buf);
}

}  // namespace bsl
