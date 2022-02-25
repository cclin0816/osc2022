.section ".text.to_kernel"
.global to_kernel

to_kernel:
  // el1 64 bit 
  mov x2, #(1 << 31)
  msr hcr_el2, x2
  // use el1 sp and no interrupts
  mov x2, #0x3c5
  msr spsr_el2, x2
  // enable fp, simd, sve
  mov x2, #(3 << 20 | 3 << 16)
  msr cpacr_el1, x2
  // stack pointer 0
  mov x2, #0
  msr sp_el1, x2 
  // kernel_addr
  msr elr_el2, x1
  // jump to kernel
  eret
