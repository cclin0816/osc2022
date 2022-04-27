#include <kn_conf.h>
#include <linker_var.h>
#include <mm.h>
#include <mm/alloc.h>
#include <mm/trans.h>
#include <mm/vm.h>

namespace mm {

void scan_mem_info() {
  auto linker_start = LINK_VAR(__linker_start);
  auto linker_end = LINK_VAR(__linker_end);

  add_rsrv_mem(0x0, 0x1000, 0);
  add_rsrv_mem(linker_start, linker_end - linker_start, 0);
  add_rsrv_mem(linker_end, KN_STACK_SZ, 0);
  add_rsrv_mem(0x8000000, 0x3D000, 0);

  add_norm_mem(0, 0x3c000000, pa::kn_mem | pa::NORM_nocache | pa::ISH);
  add_usable_mem(0, 0x3c000000, 0);
  add_norm_mem(0x3c000000, 0x3000000, pa::kn_mem | pa::NORM_nocache | pa::OSH);
  add_dev_mem(0x3f000000, 0x7e000000, 0x1000000,
              pa::kn_mem | pa::DEV_nGnRnE | pa::OSH);
  add_dev_mem(0x40000000, 0x40000000, 0x1000,
              pa::kn_mem | pa::DEV_nGnRnE | pa::OSH);

  set_kaslr(0);
}

}  // namespace mm