#include <mm.h>
#include <dt.h>
#include <excep.h>
#include <io.h>
#include <fs.h>

extern "C" uint64_t kn_entry(void *fdt_ptr) {
  dt::init(fdt_ptr);
  mm::scan_mem_info();
  mm::init_vm();
  return mm::phy_to_kn(0);
}

extern "C" void kn_vm_entry() {
  mm::init_vm_cleanup();
  mm::init_alloc();
  excep::init();
  mini_uart::init();
  fs::cpio::init((void *)0x8000000);
}
