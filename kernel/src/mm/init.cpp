// #include <bsl/endian.h>
#include <bsl/static_vec.h>
// #include <bsl/string_view.h>
// #include <dt.h>
#include <bsl/align.h>
#include <bsl/cassert.h>
#include <kn_conf.h>
#include <linker_var.h>
#include <mm.h>

namespace {

struct rsrv_mem_t {
  uint64_t addr = 0;
  uint64_t size = 0;
  uint64_t attr = 0;
  rsrv_mem_t() = default;
  rsrv_mem_t(const rsrv_mem_t &) = default;
  rsrv_mem_t(uint64_t addr, uint64_t size, uint64_t attr)
      : addr(addr), size(size), attr(attr) {}
};

uint64_t alloc_end = 0;
bsl::static_vec<rsrv_mem_t, MAX_RSRV_MEM> rsrv_mem_layout;

}  // namespace

namespace mm {

void init() {
  auto linker_start = LINK_VAR(__linker_start);
  auto linker_end = LINK_VAR(__linker_end);

  // reserve memory traversing fdt
  early_rsrv_mem(linker_start, linker_end - linker_start, 0);
  early_rsrv_mem(linker_end, KN_STACK_SZ, 0);
  // set up start position for early alloc
  alloc_end = linker_end + KN_STACK_SZ;

  // set up memory region traversing fdt
  add_norm_mem(0, 0x3c000000, pa::kn_mem | pa::NORM_nocache);
  add_norm_mem(0x3c000000, 0x3000000, pa::kn_mem | pa::NORM_nocache);
  add_dev_mem(0x3f000000, 0x7e000000, 0x1000000, pa::kn_mem | pa::DEV_nGnRnE);

  // no kaslr
  set_kaslr(0);

  // setup virtual memory
  setup_vm();
}

void *early_alloc_page(uint64_t size) {
  auto *ptr = (void *)alloc_end;
  early_rsrv_mem(alloc_end, size, 0);
  alloc_end += size;
  return ptr;
}

void early_rsrv_mem(uint64_t phy_addr, uint64_t size, uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ));
  rsrv_mem_layout.push_back(phy_addr, size, attr);
}

}  // namespace mm