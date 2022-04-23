#include <kn_conf.h>

.section ".text.boot"
.global _start
.align 4;
// assumption: at el1 and simd, sve, fp enable
_start:

  // set stack pointer
  adrp x1, __linker_end
  // 16K kernel stack
  add x1, x1, KN_STACK_SZ
  mov sp, x1
  mov x16, x0

  // init bss 
  adrp x0, __bss_start
  mov x1, 0
  adrp x2, __bss_end
  sub x2, x2, x0
  bl memset

  // jump to kernel start
  mov x0, x16
  bl kn_entry

  b jump_high

.section ".text.enable_vm"
.global enable_vm
.align 4;
jump_high:
  adr x1, jump_high_catch  
  add x1, x1, x0
  br x1
 jump_high_catch:
  bl kn_vm_entry
  b halt


.section ".text.halt"
.global halt
.align 4;
halt:
  wfe
  b halt
