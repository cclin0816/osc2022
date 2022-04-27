#include <mm.h>
#include <dt.h>
// #include <excep.h>
// #include <fs.h>
// #include <sched.h>
// #include <proc.h>
#include <conf.h>
#include <config.h>

extern "C" uint64_t kn_entry(void *fdt_ptr) {
  dt::init(fdt_ptr);
  mm::init();
  // return mm::phy_to_kn(0UL);
}

extern "C" void kn_vm_entry() {
  // mm::init_vm();
  // excep::init();
  dt::init_vm();
  // fs::init();
  // sched::init();
  // proc::thread_create(proc::idle, nullptr);
  // proc::execve("init", nullptr, nullptr);
}
