#pragma once

#include <config.h>

namespace compiler {

inline void barrier() { asm volatile("" ::: "memory"); }
inline void isb() { asm volatile("isb" ::: "memory"); }
inline void dsb() { asm volatile("dsb" ::: "memory"); }
inline void dmb() { asm volatile("dmb" ::: "memory"); }

}  // namespace compiler