#pragma once

#include <config.h>

inline constexpr bool islower(int ch) noexcept {
  return (ch >= 'a' && ch <= 'z');
}

inline constexpr bool isupper(int ch) noexcept {
  return (ch >= 'A' && ch <= 'Z');
}

inline constexpr bool isalpha(int ch) noexcept {
  return (islower(ch) || isupper(ch));
}

inline constexpr bool isdigit(int ch) noexcept {
  return (ch >= '0' && ch <= '9');
}

inline constexpr bool isxdigit(int ch) noexcept {
  return (isdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'));
}

inline constexpr bool isalnum(int ch) noexcept {
  return (isalpha(ch) || isdigit(ch));
}

inline constexpr bool ispunct(int ch) noexcept {
  return (ch >= 0x21 && ch <= 0x2f) || (ch >= 0x3a && ch <= 0x40) ||
         (ch >= 0x5b && ch <= 0x60) || (ch >= 0x7b && ch <= 0x7e);
}

inline constexpr bool isspace(int ch) noexcept {
  return (ch == '\t' || (ch >= 0x0a && ch <= 0x0d) || ch == ' ');
}

inline constexpr bool isprint(int ch) noexcept {
  return (ch >= 0x20 && ch <= 0x7e);
}