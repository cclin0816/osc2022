#pragma once

#include <cstdint>

namespace bsl {

// unsigned long rndr() {
//   unsigned long rnd;
//   asm("MRS %0, RNDR" : "=r"(rnd));
//   return rnd;
// }

constexpr uint64_t hash64(uint64_t val) {
  // using murmurhash3 64-bit finalizer
  val ^= val >> 33;
  val *= 0xff51afd7ed558ccdUL;
  val ^= val >> 33;
  val *= 0xc4ceb9fe1a85ec53UL;
  val ^= val >> 33;
  return val;
}

}  // namespace bsl