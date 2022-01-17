#pragma once

#include <cstddef>

namespace bsl {

constexpr void strcpy(char* dst, const char* src) {
  while ((*dst++ = *src++) != '\0')
    ;
}

constexpr void strncpy(char* dst, const char* src, size_t n) {
  while (n-- > 0 && (*dst++ = *src++) != '\0')
    ;
}

constexpr void strcat(char* dst, const char* src) {
  while (*dst != '\0') dst++;
  while ((*dst++ = *src++) != '\0')
    ;
}

constexpr void strncat(char* dst, const char* src, size_t n) {
  while (*dst != '\0') dst++;
  while (n-- > 0 && (*dst++ = *src++) != '\0')
    ;
}

constexpr size_t strlen(const char* str) {
  size_t len = 0;
  while (*str++ != '\0') {
    ++len;
  }
  return len;
}

constexpr int strcmp(const char* s1, const char* s2) {
  while (*s1 != '\0' && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

constexpr int strncmp(const char* s1, const char* s2, size_t n) {
  if (n == 0) {
    return 0;
  }
  while (n-- > 0 && *s1 != '\0' && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

}  // namespace bsl