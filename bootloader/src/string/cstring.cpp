#include <config.h>

extern "C" {
size_t strlen(const char *);
int strcmp(const char *, const char *);
}
size_t strlen(const char *str) {
  size_t len = 0;
  while (*str++ != '\0') {
    len++;
  }
  return len;
}

int strcmp(const char *lhs, const char *rhs) {
  while (*lhs != '\0' && *rhs != '\0') {
    if (*lhs != *rhs) break;
    lhs++;
    rhs++;
  }
  return *lhs - *rhs;
}