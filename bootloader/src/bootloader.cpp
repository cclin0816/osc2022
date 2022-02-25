#include <bsl/charconv.h>
#include <bsl/endian.h>
#include <config.h>
#include <dt.h>
#include <mmio.h>
#include <peripheral/uart.h>

#define kernel_addr 0x80000

namespace mmio {

uint64_t base = 0xfe000000;

}  // namespace mmio

void log(uint64_t num) {
  char buf[32];
  uint32_t len = bsl::to_chars(buf, sizeof(buf), num, 16);
  uart::bcm2835_aux_t serial;
  serial.write("0x", 2);
  serial.write(buf, len);
  serial.write("\n", 1);
}

NOINLINE uint64_t get_val(void *ptr, uint32_t len) {
  // log((uint64_t)ptr);
  // log(len);
  bu32_t *p = (bu32_t *)ptr;
  if (len == 1) {
    return *p;
  } else {
    uint64_t l, r;
    l = p[0];
    r = p[1];
    return (l << 32) | r;
  }
}

uint64_t get_mmio_base() {
  void *node = dt::get_node("");
  uint32_t ac_sz = dt::get_address_cells(node);
  node = dt::get_node("soc");
  uint32_t soc_ac_sz = dt::get_address_cells(node);
  uint32_t soc_sc_sz = dt::get_size_cells(node);
  auto [r_data, r_len] = dt::get_prop(node, "ranges");
  auto *ranges = (bu32_t *)r_data;
  // auto ranges = (bu32_t *)__builtin_assume_aligned(r_data, 4);
  for (auto i = 0U; i < r_len / 4;) {
    uint64_t bus_addr;
    uint64_t phys_addr;

    bus_addr = get_val(ranges + i, soc_ac_sz);
    i += soc_ac_sz;
    phys_addr = get_val(ranges + i, ac_sz);
    i += ac_sz;
    i += soc_sc_sz;
    if (bus_addr == 0x7e000000) {
      return phys_addr;
    }
  }
  return 0;
}

void hand_shake() {
  uart::bcm2835_aux_t uart;
  uint16_t start_sym = 0x5487;
  uint16_t sym = 0;
  while (sym != start_sym) {
    uint8_t c = uart.recv();
    sym = (sym << 8) | c;
  }
  uart.write<uint16_t>(0x8754);
  sym = 0;
  start_sym = 0xffff;
  while (sym != start_sym) {
    uint8_t c = uart.recv();
    sym = (sym << 8) | c;
  }
}

void recv_kernel() {
  uart::bcm2835_aux_t serial;
  while (true) {
    uint32_t kernel_size = serial.read<uint32_t>();
    uint8_t *kernel = (uint8_t *)kernel_addr;
    uint8_t check_sum = 0;

    for (auto i = 0U; i < kernel_size; i++) {
      kernel[i] = serial.recv();
      check_sum ^= kernel[i];
    }

    serial.send(check_sum);
    if (serial.recv() == 0) break;
  }
}

extern "C" [[noreturn]] void to_kernel(void *fdt_ptr, void *kernel_entry);

int main(void *fdt_ptr) {
  // get mmio base
  // initialize bcm2835_aux
  // hand shake with send_kernel.py
  // recv kernel
  // el2 to el1 at kernel entry

  dt::init(fdt_ptr);
  mmio::base = get_mmio_base();
  uart::bcm2835_aux_t uart;
  uart.init(921600);
  hand_shake();
  recv_kernel();
  to_kernel(fdt_ptr, (void *)kernel_addr);
  // dt::send_dt();
  return 0;
}