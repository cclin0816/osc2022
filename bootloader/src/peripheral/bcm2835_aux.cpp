#include <mmio.h>
#include <peripheral/uart.h>

constexpr auto GPFSEL1 = 0x200004U;

constexpr auto AUX_ENABLE = 0x215004U;
constexpr auto AUX_MU_IO = 0x215040U;
constexpr auto AUX_MU_IER = 0x215044U;
constexpr auto AUX_MU_IIR = 0x215048U;
constexpr auto AUX_MU_LCR = 0x21504CU;
constexpr auto AUX_MU_MCR = 0x215050U;
constexpr auto AUX_MU_LSR = 0x215054U;
constexpr auto AUX_MU_CNTL = 0x215060U;
constexpr auto AUX_MU_BAUD = 0x215068U;

namespace uart {

void bcm2835_aux_t::init(uint64_t baudrate) {
  mmio::w32(mmio::base, AUX_ENABLE, 1);    // enable UART1, AUX mini uart
  mmio::w32(mmio::base, AUX_MU_CNTL, 0);   // close Tx Rx
  mmio::w32(mmio::base, AUX_MU_LCR, 3);    // data size 8 bits
  mmio::w32(mmio::base, AUX_MU_MCR, 0);    // no auto flow control
  mmio::w32(mmio::base, AUX_MU_IER, 0);    // disable interrupt - enable bit
  mmio::w32(mmio::base, AUX_MU_IIR, 0x6);  // disable interrupt - clear FIFO
  mmio::w32(mmio::base, AUX_MU_BAUD, ((250000000 / 8) - baudrate) / baudrate);

  // setup gpio pin

  unsigned int r;
  r = mmio::r32(mmio::base, GPFSEL1);
  r &= ~((7 << 12) | (7 << 15));  // clear gpio14, gpio15 setting
  r |= (2 << 12) | (2 << 15);     // set alt5
  mmio::w32(mmio::base, GPFSEL1, r);

  // pi3 pi4 gpio differs, and reset value is fine
  //   // mmio::w32(GPPUD, 0);  // disable pull-up/down
  //   // wait_clock(150);
  //   // mmio::w32(GPPUDCLK0, (1 << 14) | (1 << 15));  // set to gpio14, gpio15
  //   // wait_clock(150);
  //   // mmio::w32(GPPUDCLK0, 0);    // flush GPIO setup
  //   // r = mmio::r32(GPIO_PUP_PDN_CNTRL_REG0);
  //   // r &= ~((3 << 30) | (3 << 28));
  //   // mmio::w32(GPIO_PUP_PDN_CNTRL_REG0, r);

  mmio::w32(mmio::base, AUX_MU_CNTL, 3);  // enable Tx Rx
}

void bcm2835_aux_t::wait_recv() {
  while ((mmio::r32(mmio::base, AUX_MU_LSR) & 0x1) == 0)
    ;
}

uint8_t bcm2835_aux_t::recv_raw() {
  return (uint8_t)(mmio::r32(mmio::base, AUX_MU_IO));
}

uint8_t bcm2835_aux_t::recv() {
  wait_recv();
  return recv_raw();
}

void bcm2835_aux_t::wait_send() {
  while ((mmio::r32(mmio::base, AUX_MU_LSR) & 0x20) == 0)
    ;
}

void bcm2835_aux_t::send_raw(const uint8_t data) {
  mmio::w32(mmio::base, AUX_MU_IO, data);
}

void bcm2835_aux_t::send(const uint8_t data) {
  wait_send();
  send_raw(data);
}

}  // namespace uart