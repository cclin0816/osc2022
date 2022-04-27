#include <bsl/string_view.h>
#include <conf.h>
#include <config.h>
#include <dt.h>
#include <excep.h>
#include <fs.h>
#include <mm.h>
#include <proc.h>
#include <schd.h>

using namespace bsl::svl;

extern "C" uint64_t kn_entry(void *fdt_ptr) {
  dt::init(fdt_ptr);
  mm::init();
  return mm::phy_to_kn(0UL);
}

extern "C" void kn_vm_entry() {
  mm::init_vm();
  excep::init();
  dt::init_vm();
  // fs::init();
  // sched::init();
  // proc::thread_create(proc::idle, nullptr);
  // proc::execve("init", nullptr, nullptr);
  auto [fp, len] = fs::cpio::get_file("tt"sv);
  auto [fp2, len2] = fs::cpio::get_file("tt2"sv);
}
