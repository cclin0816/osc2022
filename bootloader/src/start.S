// assumption: 
// bootlaoder at >= 0x60000
// <- 0x60000 -> is usable
// section is 16 byte aligned

// layout:
// low | <- stack | 0x60000 | bootloader -> | high

.section ".text.boot"
.global _start

_start:
  b _setup

// at 0x60004, for relay to main
_to_main:
  // load arguments
  ldp x0, x1, [sp], #16
  ldp x2, x3, [sp], #16
  // jump to main
  bl _Z4mainPv
  // return from main
  b halt

_setup:

  // set stack pointer
  mov sp, #0x60000

  // store arguments passed to kernel
  stp x2, x3, [sp, #-16]!
  stp x0, x1, [sp, #-16]!

  // relocate bootloader
  // x0 loop cnt
  ldr x0, =__bootloader_sz
  // x1 load addr
  adr x1, _start
  // x2 store addr
  mov x2, #0x60000

_reloc_loop:
  ldp x3, x4, [x1], #16
  stp x3, x4, [x2], #16
  sub x0, x0, #1
  cbnz x0, _reloc_loop

  // initialize bss
  // x0 bss start addr
  ldr x0, =__bss_offset
  add x0, x0, #0x60000
  // x1 loop cnt
  ldr x1, =__bss_sz
  // jump to main
  cbz x1, _relay_to_main

_bss_init_loop:
  stp xzr, xzr, [x0], #16
  sub x1, x1, #1
  cbnz x1, _bss_init_loop

_relay_to_main:
  mov x0, #0x60000
  add x0, x0, #0x4
  br x0

.section ".text.halt"

.global halt

halt:
  wfe
  b halt
