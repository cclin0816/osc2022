.macro save_all
    stp x0 , x1 , [sp, #-16]!
    stp x2 , x3 , [sp, #-16]!
    stp x4 , x5 , [sp, #-16]!
    stp x6 , x7 , [sp, #-16]!
    stp x8 , x9 , [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    str x30, [sp, #-16]!
    stp q0, q1, [sp, #-32]!
    stp q2, q3, [sp, #-32]!
    stp q4, q5, [sp, #-32]!
    stp q6, q7, [sp, #-32]!
    stp q8, q9, [sp, #-32]!
    stp q10, q11, [sp, #-32]!
    stp q12, q13, [sp, #-32]!
    stp q14, q15, [sp, #-32]!
    stp q16, q17, [sp, #-32]!
    stp q18, q19, [sp, #-32]!
    stp q20, q21, [sp, #-32]!
    stp q22, q23, [sp, #-32]!
    stp q24, q25, [sp, #-32]!
    stp q26, q27, [sp, #-32]!
    stp q28, q29, [sp, #-32]!
    stp q30, q31, [sp, #-32]!
    mrs x1, elr_el1
    mrs x2, spsr_el1
    stp x1, x2 , [sp, #-16]!
.endm

.macro load_all
    ldp x1, x2 , [sp], #16
    msr elr_el1, x1
    msr spsr_el1, x2
    ldp q30, q31, [sp], #32
    ldp q28, q29, [sp], #32
    ldp q26, q27, [sp], #32
    ldp q24, q25, [sp], #32
    ldp q22, q23, [sp], #32
    ldp q20, q21, [sp], #32
    ldp q18, q19, [sp], #32
    ldp q16, q17, [sp], #32
    ldp q14, q15, [sp], #32
    ldp q12, q13, [sp], #32
    ldp q10, q11, [sp], #32
    ldp q8, q9, [sp], #32
    ldp q6, q7, [sp], #32
    ldp q4, q5, [sp], #32
    ldp q2, q3, [sp], #32
    ldp q0, q1, [sp], #32
    ldr x30, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x0, x1, [sp], #16
.endm

.section ".text.vec_tbl"
.align 11

vec_tbl:
  // from curerent el while using sp_el0
  .align 7
  b _excep_not_impl // sync
  .align 7
  b _excep_not_impl // irq
  .align 7
  b _excep_not_impl // fiq
  .align 7
  b _excep_not_impl // serror
  // from curerent el while using sp_elx
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  // from lower el aarch64
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  // from lower el aarch32
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl
  .align 7
  b _excep_not_impl 

.section ".text.set_vec_tbl"
.global set_vec_tbl
.align 4

set_vec_tbl:
  adr x0, vec_tbl
  msr vbar_el1, x0
  ret

.section ".text._irq_handler"
.global _irq_handler
.align 4

_irq_handler:
  save_all
  bl irq_handler
  load_all
  eret

.section ".text._sync_handler"
.global _sync_handler
.align 4

_sync_handler:
  save_all
  mov x0, sp
  mrs x1, esr_el1
  bl sync_handler
  load_all
  eret

.section ".text._excep_not_impl"
.global _excep_not_impl
.align 4

_excep_not_impl:
  save_all
  bl excep_not_impl
  load_all
  eret
