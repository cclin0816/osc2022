#pragma once
#include <config.h>
#include <bsl/pair.h>

namespace mailbox {

// void req(void *msg, uint8_t channel);
uint32_t get_firm_rev();
uint32_t get_board_model();
uint32_t get_board_rev();
uint64_t get_board_mac();
uint64_t get_board_serial();
bsl::pair_t<uint32_t, uint32_t> get_arm_mem();
bsl::pair_t<uint32_t, uint32_t> get_vc_mem();

}  // namespace mailbox
