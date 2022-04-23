#pragma once

#include <config.h>

namespace bsl {

// unsigned long rndr() {
//   unsigned long rnd;
//   asm("MRS %0, RNDR" : "=r"(rnd));
//   return rnd;
// }

constexpr uint64_t hash64(uint64_t val) {
  val ^= (val >> 28 | val << 36);
  val ^= 0x211b6a6b589a5a2c;
  val ^= (val >> 7 | val << 57);
  val ^= 0xddb4f8f6abce9838;
  val ^= (val >> 45 | val << 19);
  val ^= 0x11c6398cbb55b99f;
  return val;
}

constexpr uint8_t hash8(uint64_t val) {
  val = hash64(val);
  auto *arr = (uint8_t *)&val;
    return arr[0] ^ arr[1] ^ arr[2] ^ arr[3];
}

}  // namespace bsl