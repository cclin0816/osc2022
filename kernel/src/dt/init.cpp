#include <dt.h>
#include <fs/cpio.h>
#include <linker_var.h>
#include <mm/alloc.h>
#include <mm/trans.h>
#include <mm/vm.h>

namespace {

void *fdt_addr = nullptr;

}

namespace dt {

void init(void *fdt_ptr) {
  fdt_addr = fdt_ptr;

  mm::set_kaslr(0);

  auto linker_start = LINK_VAR(__linker_start);
  auto linker_end = LINK_VAR(__linker_end);

  mm::add_rsrv_mem(0x0, 0x1000, 0);
  mm::add_rsrv_mem(linker_start, linker_end - linker_start, 0);
  mm::add_rsrv_mem(linker_end, KN_STACK_SZ, 0);
  mm::add_rsrv_mem(0x8000000, 0x40000, 0);
  fs::cpio::set_base(0x8000000);

  mm::add_norm_mem(0, 0x3c000000,
                   mm::pa::kn_mem | mm::pa::NORM_nocache | mm::pa::ISH);
  mm::add_usable_mem(0, 0x3c000000, 0);
  mm::add_norm_mem(0x3c000000, 0x3000000,
                   mm::pa::kn_mem | mm::pa::NORM_nocache | mm::pa::OSH);
  mm::add_dev_mem(0x3f000000, 0x7e000000, 0x1000000,
                  mm::pa::kn_mem | mm::pa::DEV_nGnRnE | mm::pa::OSH);
  mm::add_dev_mem(0x40000000, 0x40000000, 0x1000,
                  mm::pa::kn_mem | mm::pa::DEV_nGnRnE | mm::pa::OSH);
}

void init_vm() {}

}  // namespace dt