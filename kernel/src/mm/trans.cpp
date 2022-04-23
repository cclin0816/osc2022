#include <mm.h>

namespace {

bool vm_enable = false;
uint64_t kn_base = 0;
constexpr auto high_addr = 0xffff000000000000UL;
// 40 bit physical address space
//  8 bit aslr
// 48 bit virtual address space
constexpr auto kn_base_mask = 0xffffff0000000000UL;

inline bool is_kn_addr(uint64_t addr) {
  return (addr & kn_base_mask) == kn_base;
}

}  // namespace

namespace mm {
// direct map
uint64_t phy_to_kn(uint64_t addr) { return addr + kn_base; }

uint64_t kn_to_phy(uint64_t addr) { return addr - kn_base; }

uint64_t to_kn(uint64_t addr) {
  if (is_kn_addr(addr)) {
    return addr;
  } else {
    return phy_to_kn(to_phy(addr));
  }
}

uint64_t to_phy(uint64_t addr) {
  if (is_kn_addr(addr)) {
    return kn_to_phy(addr);
  } else {
    return addr;
  }
}

uint64_t to_access(uint64_t addr) {
  if ((is_kn_addr(addr) && vm_enable) || !(is_kn_addr(addr) || vm_enable)) {
    return addr;
  } else if (is_kn_addr(addr)) {
    return to_phy(addr);
  } else {
    return to_kn(addr);
  }
}

void set_kaslr(uint64_t aslr) { kn_base = (aslr & kn_base_mask) | high_addr; }

}  // namespace mm