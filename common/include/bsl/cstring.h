#pragma once

#include <cstddef>

namespace bsl {

constexpr void *memchr(const void *ptr, int ch, size_t count) noexcept {
  return __builtin_memchr(ptr, ch, count);
}

constexpr int memcmp(const void *lhs, const void *rhs, size_t count) noexcept {
  return __builtin_memcmp(lhs, rhs, count);
}

constexpr char *strchr(const char *str, int ch) noexcept {
  return __builtin_strchr(str, ch);
}

constexpr int strcmp(const char *lhs, const char *rhs) noexcept {
  return __builtin_strcmp(lhs, rhs);
}

constexpr size_t strlen(const char *str) noexcept {
  return __builtin_strlen(str);
}

constexpr int strncmp(const char *lhs, const char *rhs, size_t count) noexcept {
  return __builtin_strncmp(lhs, rhs, count);
}

constexpr void *memcpy(void *dest, const void *src, size_t count) noexcept {
  return __builtin_memcpy(dest, src, count);
}

constexpr void *memmove(void *dest, const void *src, size_t count) noexcept {
  return __builtin_memmove(dest, src, count);
}

#define inline_memcpy(dest, src, count) \
  __builtin_memcpy_inline(dest, src, count)

}  // namespace bsl