#pragma once

#include <config.h>

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

constexpr void *memcpy(void *dst, const void *src, size_t count) noexcept {
  return __builtin_memcpy(dst, src, count);
}

constexpr void *memmove(void *dst, const void *src, size_t count) noexcept {
  return __builtin_memmove(dst, src, count);
}

inline void *memset(void *dst, int ch, size_t count) noexcept {
  return __builtin_memset(dst, ch, count);
}

#define inline_memcpy(dst, src, count) __builtin_memcpy_inline(dst, src, count)

constexpr size_t strlen_slow(const char *str) noexcept {
  size_t len = 0;
  while (str[len] != '\0') {
    ++len;
  }
  return len;
}

}  // namespace bsl