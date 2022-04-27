#pragma once

#include <cstddef>
#include <cstdint>

constexpr auto CACHELINE_SIZE = 64U;

#define ALIGN(x) __attribute__((aligned(x)))
#define CACHELINE_ALIGN ALIGN(CACHELINE_SIZE)
#define INLINE __attribute__((inline))
#define FORCE_INLINE __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define ASSUME(x) __builtin_assume(x)
#define PACKED __attribute__((packed))
#define PURE __attribute__((pure))
