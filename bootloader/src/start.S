// assumption: 
// bootlaoder at >= 0x60000
// <- 0x60000 -> is usable
// section is 16 byte aligned

// layout:
// low | <- stack | 0x60000 | bootloader -> | high

#define reloc_addr #0x60000

// fixing vtable
// only fix vtable if reloc_addr differs from linker.ld __linker_start

.section ".text.boot"
.global _start

_start:
  b _setup

// at reloc_addr + 4, for relay to main
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
  mov sp, reloc_addr

  // store arguments passed to kernel
  stp x2, x3, [sp, #-16]!
  stp x0, x1, [sp, #-16]!

  // relocate bootloader
  // x0 loop cnt
  ldr x0, =__bootloader_sz
  // x1 load addr
  adr x1, _start
  // x2 store addr
  mov x2, reloc_addr

_reloc_loop:
  ldp x3, x4, [x1], #16
  stp x3, x4, [x2], #16
  sub x0, x0, #1
  cbnz x0, _reloc_loop

  // initialize bss
  // x0 bss start addr
  ldr x0, =__bss_offset
  add x0, x0, reloc_addr
  // x1 loop cnt
  ldr x1, =__bss_sz
  // jump to main
  cbz x1, _fix_vtable

_bss_init_loop:
  stp xzr, xzr, [x0], #16
  sub x1, x1, #1
  cbnz x1, _bss_init_loop

_fix_vtable:
//   ldr x0, =__linker_start
//   mov x1, reloc_addr
//   // if differ then fix vtable
//   sub x0, x1, x0
//   cbz x0, _relay_to_main
//   ldr x1, =__vtable_offset
//   add x1, x1, reloc_addr
//   ldr x2, =__vtable_sz
//   cbz x2, _relay_to_main

// _fix_vtable_loop:
//   ldr x3, [x1]
//   add x3, x3, x0
//   str x3, [x1], #8
//   sub x2, x2, #1
//   cbnz x2, _fix_vtable_loop

_relay_to_main:
  mov x0, reloc_addr
  add x0, x0, #0x4
  br x0

.section ".text.halt"

.global halt

halt:
  wfe
  b halt

.section ".text.halt"

.global __cxa_pure_virtual

__cxa_pure_virtual:
  b halt