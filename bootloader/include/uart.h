#pragma once

#include <config.h>

namespace uart0 {

void init();
uint8_t recv();
void recv(char *buf, uint64_t sz);
void send(uint8_t data);
void send(const char *data, uint64_t sz);
void send(const char *str);
void flush();

// I know these are UB, but codegen is fine
template <typename T, size_t sz = sizeof(T)>
T read() {
  T buf;
  recv(reinterpret_cast<char *>(&buf), sz);
  return buf;
}

template <typename T, size_t sz = sizeof(T)>
void write(const T data) {
  send(reinterpret_cast<const char *>(&data), sz);
}

}  // namespace uart0