#include <excep.h>
#include <excep/excep_entry.h>

namespace {

bool excep_enabled = false;

}

namespace excep {

void init() {
  set_vec_tbl();
  enable();
}

void enable() {
  excep_enabled = true;
  asm volatile("msr DAIFClr, 0xf");
}

void disable() {
  asm volatile("msr DAIFSet, 0xf");
  excep_enabled = false;
}

bool is_enabled() { return excep_enabled; }

}  // namespace excep