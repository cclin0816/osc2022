#pragma once

#include <bsl/string_view.h>

namespace bsl {

// CRTP extension for serial charachter device
// need only recv and send function
template <typename Tp>
class char_dev_t {
 public:
  void read(char *buf, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
      buf[i] = static_cast<Tp *>(this)->recv();
    }
  }
  void write(const char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
      static_cast<Tp *>(this)->send(buf[i]);
    }
  }
  void write(const char *buf) {
    while (*buf != '\0') {
      static_cast<Tp *>(this)->send(*buf++);
    }
  }
  void write(bsl::string_view_t sv) { write(sv.data(), sv.size()); }
  template <typename T, size_t sz = sizeof(T)>
  T read() {
    T buf;
    read((char *)(&buf), sz);
    return buf;
  }

  template <typename T, size_t sz = sizeof(T)>
  void write(const T data) {
    write((const char *)(&data), sz);
  }
};

}  // namespace bsl