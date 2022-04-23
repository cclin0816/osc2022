#include <mm.h>
#include <dt.h>

// entry after _start
// setup virtual memory
extern "C" uint64_t kn_entry(void *fdt_ptr) {
  dt::init(fdt_ptr);
  mm::init();
  return mm::phy_to_kn(0);
}

// entry after enable_vm
extern "C" void kn_vm_entry() {

}
