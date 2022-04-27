#pragma once

#include <config.h>

inline void barrier() { asm volatile("" ::: "memory"); }
inline void isb() { asm volatile("isb" ::: "memory"); }
#define dsb(sh) asm volatile("dsb " #sh ::: "memory")
#define dmb(sh) asm volatile("dmb " #sh ::: "memory")
#define tlb_flush(sh) asm volatile("tlbi vmalle1"#sh ::: "memory")
