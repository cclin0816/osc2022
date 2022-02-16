
// low | bootloader (kernel - 0x20000)-> | stack <-(kernel) | kernel -> | high

.section ".text.boot"

.global _start

_start:

  // get _start real address
  adr x4, _start
  // set stack to start of kernel image
  mov sp, x4
  // store arguments passed to kernel
  stp x2, x3, [sp, #-16]!
  stp x0, x1, [sp, #-16]!

  // relocate bootloader
  ldr x0, =__bootloader_sz
  mov x1, x4
  sub x2, x4, #0x20000

_reloc_loop:
  ldp x3, x5, [x1], #16
  stp x3, x5, [x2], #16
  sub x0, x0, #1
  cbnz x0, _reloc_loop

  // initialize bss
  ldr x0, =__bss_offset
  add x0, x0, x4
  sub x0, x0, #0x20000
  ldr x1, =__bss_sz
  cbz x1, _to_main

_bss_init_loop:
  stp xzr, xzr, [x0], #16
  sub x1, x1, #1
  cbnz x1, _bss_init_loop

_to_main:
  // jump to main
  bl _Z4mainv - 0x20000
  // return from main
  b _halt


.section ".text.halt"

.global _halt

_halt:
  wfe
  b _halt
