// #include <bsl/charconv.h>
#include <config.h>
// #include <mailbox.h>
#include <peripheral/uart.h>
// #include <watchdog.h>
#include <fdt.h>

// extern "C" {
// [[noreturn]] void _halt();
// }

// namespace bl {

// bool strcmp(const char *lhs, const char *rhs) {
//   while (*lhs && *rhs) {
//     if (*lhs != *rhs) {
//       return false;
//     }
//     ++lhs;
//     ++rhs;
//   }
//   return *lhs == *rhs;
// }

// template <typename T>
// void log_num(const char *str, T num) {
//   char buf[64];
//   uint32_t len = bsl::to_chars(buf, sizeof(buf), num, 16);
//   uart0::send(str);
//   uart0::send(buf, len);
//   uart0::send("\n");
// }

// }  // namespace bl

// void shell() {
//   while (true) {
//     // uart0::send("> ");
//     char cmd[64];
//     uint64_t pos = 0;
//     while (true) {
//       char c = uart0::recv();
//       if (c == '\n') {
//         cmd[pos] = '\0';
//         break;
//       }
//       cmd[pos++] = c;
//     }
//     if (pos == 0) {
//       continue;
//     } else if (bl::strcmp(cmd, "help")) {
//       uart0::send("help: \n help\n hello\n reboot\n board rev\n arm memory\n");
//     } else if (bl::strcmp(cmd, "hello")) {
//       uart0::send("hello\n");
//     } else if (bl::strcmp(cmd, "reboot")) {
//       uart0::send("reboot\n");
//       uart0::flush();
//       watchdog::start(5);
//       _halt();
//     } else if (bl::strcmp(cmd, "board rev")) {
//       bl::log_num("board rev: 0x", mailbox::get_board_rev());
//     } else if (bl::strcmp(cmd, "arm memory")) {
//       auto [mem_base, mem_sz] = mailbox::get_arm_mem();
//       bl::log_num("arm mem size: 0x", mem_sz);
//       bl::log_num("arm mem base: 0x", mem_base);
//     } else if (bl::strcmp(cmd, "firm rev")) {
//       bl::log_num("firm rev: 0x", mailbox::get_firm_rev());
//     } else if (bl::strcmp(cmd, "board model")) {
//       bl::log_num("board model: 0x", mailbox::get_board_model());
//     } else if (bl::strcmp(cmd, "board mac")) {
//       bl::log_num("board mac: 0x", mailbox::get_board_mac());
//     } else if (bl::strcmp(cmd, "board serial")) {
//       bl::log_num("board serial: 0x", mailbox::get_board_serial());
//     } else if (bl::strcmp(cmd, "vc memory")) {
//       auto [mem_base, mem_sz] = mailbox::get_vc_mem();
//       bl::log_num("vc mem size: 0x", mem_sz);
//       bl::log_num("vc mem base: 0x", mem_base);
//     }  else {
//       uart0::send("unknown command\n");
//     }
//   }
// }

int main(void *fdt_ptr) {

  uart0::init();

  // wait uart connect
  // uart0::recv();

  fdt::walk(fdt_ptr);

  // shell();

  return 0;
}