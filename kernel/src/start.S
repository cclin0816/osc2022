#include <kn_conf.h>

.section ".text.boot"
.global _start
.align 4;
// assumption: at el1 and simd, sve, fp enable
_start:

  // lower to el1
  // 64 bit 
  mov x2, #(1 << 31)
  msr hcr_el2, x2
  // use el1 sp and no interrupts
  mov x2, #0x3c5
  msr spsr_el2, x2
  // enable fp, simd, sve
  mov x2, #(3 << 20 | 3 << 16)
  msr cpacr_el1, x2 
  // kernel_addr
  adr x1, _el1_start
  msr elr_el2, x1
  // jump to kernel
  eret

_el1_start:

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
  add sp, sp, x0
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
