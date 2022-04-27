#include <bsl/align.h>
#include <kn_conf.h>
#include <mm/trans.h>

namespace {

bool vm_enable = false;
uint64_t kn_base = 0;
constexpr auto high_addr = 0xffffffffffffffffUL << HIGH_ADDR_SZ;
constexpr auto max_aslr = 0xffffffffffffffffUL << PHY_ADDR_SZ;
inline bool is_kn_addr(uint64_t addr) {
  return (addr & high_addr) == high_addr;
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
    return phy_to_kn(addr);
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
  bool is_kn = is_kn_addr(addr);
  if ((is_kn && vm_enable) || !(is_kn || vm_enable)) {
    return addr;
  } else if (is_kn) {
    return kn_to_phy(addr);
  } else {
    return phy_to_kn(addr);
  }
}

void set_kaslr(uint64_t aslr) {
  aslr = bsl::p2align_down(aslr, PAGE_SZ);
  aslr = aslr | high_addr;
  if (aslr > max_aslr) {
    aslr = high_addr;
  }
  kn_base = aslr;
}

void set_vm_enable() { vm_enable = true; }

uint64_t bus_to_kn(uint64_t addr) {
  auto pa = bus_to_phy(addr);
  if (pa == IDX_FAIL) {
    return IDX_FAIL;
  }
  return phy_to_kn(pa);
}

}  // namespace mm