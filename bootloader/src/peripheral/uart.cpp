#include <mmio.h>
#include <peripheral/uart.h>

namespace uart0 {

constexpr auto GPFSEL1 = 0x00200004U;
// constexpr auto GPPUD =  0x00200094U;
// constexpr auto GPPUDCLK0 = 0x00200098U;
// constexpr auto GPIO_PUP_PDN_CNTRL_REG0 = 0x002000e4U;

constexpr auto AUX_ENABLE = 0x00215004U;
constexpr auto AUX_MU_IO = 0x00215040U;
constexpr auto AUX_MU_IER = 0x00215044U;
constexpr auto AUX_MU_IIR = 0x00215048U;
constexpr auto AUX_MU_LCR = 0x0021504CU;
constexpr auto AUX_MU_MCR = 0x00215050U;
constexpr auto AUX_MU_LSR = 0x00215054U;
// constexpr auto AUX_MU_MSR = 0x00215058U;
// constexpr auto AUX_MU_SCRATCH = 0x0021505CU;
constexpr auto AUX_MU_CNTL = 0x00215060U;
// constexpr auto AUX_MU_STAT = 0x00215064U;
constexpr auto AUX_MU_BAUD = 0x00215068U;

void init() {
  // initialize UART
  mmio::w32(AUX_ENABLE, 1);     // enable UART1, AUX mini uart
  mmio::w32(AUX_MU_CNTL, 0);    // close Tx Rx
  mmio::w32(AUX_MU_LCR, 3);     // data size 8 bits
  mmio::w32(AUX_MU_MCR, 0);     // no auto flow control
  mmio::w32(AUX_MU_IER, 0);     // disable interrupt - enable bit
  mmio::w32(AUX_MU_IIR, 0x6);   // disable interrupt - clear FIFO
  mmio::w32(AUX_MU_BAUD, 33);  // 115200 baud

  // map UART1 to GPIO pins
  unsigned int r;
  r = mmio::r32(GPFSEL1);
  r &= ~((7 << 12) | (7 << 15));  // clear gpio14, gpio15 setting
  r |= (2 << 12) | (2 << 15);     // set alt5
  mmio::w32(GPFSEL1, r);
  // mmio::w32(GPPUD, 0);  // disable pull-up/down
  // wait_clock(150);
  // mmio::w32(GPPUDCLK0, (1 << 14) | (1 << 15));  // set to gpio14, gpio15
  // wait_clock(150);
  // mmio::w32(GPPUDCLK0, 0);    // flush GPIO setup
  // r = mmio::r32(GPIO_PUP_PDN_CNTRL_REG0);
  // r &= ~((3 << 30) | (3 << 28)); 
  // mmio::w32(GPIO_PUP_PDN_CNTRL_REG0, r);
  
  mmio::w32(AUX_MU_CNTL, 3);  // enable Tx Rx
}

/**
 * @brief wait uart0 rx fifo not empty
 */
void wait_readable() {
  while ((mmio::r32(AUX_MU_LSR) & 0x1) == 0)
    ;
}

/**
 * @brief read byte from uart0, if not readable return value is undefined
 * @return uint8_t
 */
uint8_t recv_raw() { return (uint8_t)(mmio::r32(AUX_MU_IO)); }

/**
 * @brief blocked read byte from uart0
 * @return uint8_t
 */
uint8_t recv() {
  wait_readable();
  return recv_raw();
}

/**
 * @brief blocked read string from uart0
 * @param buf string buffer
 * @param sz read length
 */
void recv(char *buf, const uint64_t sz) {
  for (uint64_t i = 0; i < sz; ++i) buf[i] = recv();
}

/**
 * @brief wait uart0 tx fifo not full
 */
void wait_writable() {
  while ((mmio::r32(AUX_MU_LSR) & 0x20) == 0)
    ;
}

/**
 * @brief write byte to uart0, if not writable result undefined
 * @param data byte to write
 */
void send_raw(const uint8_t data) { mmio::w32(AUX_MU_IO, data); }

/**
 * @brief blocked write byte to uart0
 * @param data byte to write
 */
void send(const uint8_t data) {
  wait_writable();
  send_raw(data);
}

/**
 * @brief blocked write string to uart0
 * @param data string to write
 * @param sz write length
 */
void send(const char *data, const uint64_t sz) {
  for (uint64_t i = 0; i < sz; ++i) send(data[i]);
}

/**
 * @brief blocked write string to uart0
 * @param data null terminate string to write
 */
void send(const char *str) {
  while (*str) send(*str++);
}

/**
 * @brief blocked until uart0 tx fifo empty
 */
void flush() {
  while ((mmio::r32(AUX_MU_LSR) & (1 << 9)) == 0)
    ;
}

}  // namespace uart0