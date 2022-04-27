#include <io.h>
#include <mm.h>
#include <mmio.h>

namespace {

uint64_t gpio_base = 0;
uint64_t mini_uart_base = 0;
uint64_t aux_base = 0;
constexpr auto GPFSEL1 = 0x4UL;
constexpr auto AUX_ENABLE = 0x4UL;
constexpr auto AUX_MU_IO = 0x0UL;
constexpr auto AUX_MU_IER = 0x4UL;
constexpr auto AUX_MU_IIR = 0x8UL;
constexpr auto AUX_MU_LCR = 0xCUL;
constexpr auto AUX_MU_MCR = 0x10UL;
constexpr auto AUX_MU_LSR = 0x14UL;
constexpr auto AUX_MU_CNTL = 0x20UL;
constexpr auto AUX_MU_BAUD = 0x28UL;

}  // namespace

namespace mini_uart {

void init(uint32_t baudrate) {
  gpio_base = mm::bus_to_kn(0x7e200000);
  aux_base = mm::bus_to_kn(0x7e215000);
  mini_uart_base = mm::bus_to_kn(0x7e215040);
  mmio::w32(aux_base, AUX_ENABLE, 1U);         // enable UART1, AUX mini uart
  mmio::w32(mini_uart_base, AUX_MU_CNTL, 0U);  // close Tx Rx
  mmio::w32(mini_uart_base, AUX_MU_LCR, 3U);   // data size 8 bits
  mmio::w32(mini_uart_base, AUX_MU_MCR, 0U);   // no auto flow control
  mmio::w32(mini_uart_base, AUX_MU_IER, 0U);   // disable interrupt - enable bit
  mmio::w32(mini_uart_base, AUX_MU_IIR,
            0x6U);  // disable interrupt - clear FIFO
  mmio::w32(mini_uart_base, AUX_MU_BAUD,
            ((250000000U / 8U) - baudrate) / baudrate);

  unsigned int r;
  r = mmio::r32(gpio_base, GPFSEL1);
  r &= ~((7U << 12U) | (7U << 15U));  // clear gpio14, gpio15 setting
  r |= (2U << 12U) | (2U << 15U);     // set alt5
  mmio::w32(gpio_base, GPFSEL1, r);

  mmio::w32(mini_uart_base, AUX_MU_CNTL, 3);  // enable Tx Rx
}

}  // namespace mini_uart