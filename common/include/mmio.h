#pragma once

#include <config.h>

namespace mmio {

extern uint64_t base;

inline uint64_t r64(void *addr) { return *(volatile uint64_t *)addr; }

inline void w64(void *addr, uint64_t val) { *(volatile uint64_t *)addr = val; }

inline uint32_t r32(void *addr) { return *(volatile uint32_t *)addr; }

inline void w32(void *addr, uint32_t val) { *(volatile uint32_t *)addr = val; }

inline uint16_t r16(void *addr) { return *(volatile uint16_t *)addr; }

inline void w16(void *addr, uint16_t val) { *(volatile uint16_t *)addr = val; }

inline uint8_t r8(void *addr) { return *(volatile uint8_t *)addr; }

inline void w8(void *addr, uint8_t val) { *(volatile uint8_t *)addr = val; }

inline uint64_t r64(uint64_t offset) { return r64((void *)(base + offset)); }

inline void w64(uint64_t offset, uint64_t val) {
  w64((void *)(base + offset), val);
}

inline uint32_t r32(uint64_t offset) { return r32((void *)(base + offset)); }

inline void w32(uint64_t offset, uint32_t val) {
  w32((void *)(base + offset), val);
}

inline uint16_t r16(uint64_t offset) { return r16((void *)(base + offset)); }

inline void w16(uint64_t offset, uint16_t val) {
  w16((void *)(base + offset), val);
}

inline uint8_t r8(uint64_t offset) { return r8((void *)(base + offset)); }

inline void w8(uint64_t offset, uint8_t val) {
  w8((void *)(base + offset), val);
}

}  // namespace mmio