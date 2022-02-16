#pragma once

#include <cstddef>
#include <cstdint>

constexpr auto CACHELINE_SIZE = 64U;

#define CACHELINE_ALIGN __attribute__((aligned(CACHELINE_SIZE)))
#define INLINE inline
#define FORCE_INLINE __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define ASSUME(x) __builtin_assume(x)

// struct CACHELINE_ALIGN cacheline_t {
//   char pad[CACHELINE_SIZE];
// };