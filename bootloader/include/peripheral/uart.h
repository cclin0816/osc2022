#pragma once

#include <config.h>
#include <bsl/string_view.h>

namespace uart {

class interface_t {
 public:
  virtual void send(uint8_t c) = 0;
  virtual uint8_t recv() = 0;
  // virtual void flush() = 0;
  void read(char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
      buf[i] = recv();
    }
  }
  void write(const char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
      send(buf[i]);
    }
  }
  void write(const char *buf) {
    while (*buf) {
      send(*buf++);
    }
  }
  void write(bsl::string_view_t sv) {
    write(sv.data(), sv.size());
  }
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

class bcm2835_aux_t : public interface_t {
 private:
  // uint64_t base;

 public:
  virtual void send(uint8_t c) override;
  virtual uint8_t recv() override;
  void wait_send();
  void wait_recv();
  void send_raw(uint8_t c);
  uint8_t recv_raw();
  // bcm2835_aux_t(uint64_t mmio_base)
  //     : base(mmio_base) {}
  void init(uint64_t baudrate = 115200);
};

class dummy_t : public interface_t {
 public:
  virtual void send([[maybe_unused]] uint8_t c) override {}
  virtual uint8_t recv() override { return 0; }
};

}  // namespace uart