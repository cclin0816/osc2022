.section ".text.boot"
.global _start

_start:

  // set stack pointer
  ldr x1, =__bss_end
  // 16K kernel stack
  add x1, x1, #0x4000
  mov sp, x1
  mov x16, x0

  // init bss 
  ldr x0, =__bss_start
  mov x1, 0
  ldr x2, =__bss_sz
  bl memset

  // jump to kernel
  mov x0, x16
  bl _Z4mainPv

  b halt 

.section ".text.halt"

.global halt

halt:
  wfe
  b halt
