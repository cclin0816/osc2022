#pragma once

#include <config.h>

#include <atomic>

namespace mmio {

extern uint64_t base;

inline uint64_t r64(void *addr,
                    std::memory_order order = std::memory_order_relaxed) {
  return (*(volatile std::atomic<uint64_t> *)addr).load(order);
}

inline void w64(void *addr, uint64_t val,
                std::memory_order order = std::memory_order_relaxed) {
  (*(volatile std::atomic<uint64_t> *)addr).store(val, order);
}

inline uint32_t r32(void *addr,
                    std::memory_order order = std::memory_order_relaxed) {
  return (*(volatile std::atomic<uint32_t> *)addr).load(order);
}

inline void w32(void *addr, uint32_t val,
                std::memory_order order = std::memory_order_relaxed) {
  (*(volatile std::atomic<uint32_t> *)addr).store(val, order);
}

inline uint16_t r16(void *addr,
                    std::memory_order order = std::memory_order_relaxed) {
  return (*(volatile std::atomic<uint16_t> *)addr).load(order);
}

inline void w16(void *addr, uint16_t val,
                std::memory_order order = std::memory_order_relaxed) {
  (*(volatile std::atomic<uint16_t> *)addr).store(val, order);
}

inline uint8_t r8(void *addr,
                  std::memory_order order = std::memory_order_relaxed) {
  return (*(volatile std::atomic<uint8_t> *)addr).load(order);
}

inline void w8(void *addr, uint8_t val,
               std::memory_order order = std::memory_order_relaxed) {
  (*(volatile std::atomic<uint8_t> *)addr).store(val, order);
}

inline uint64_t r64(uint64_t base, uint64_t offset,
                    std::memory_order order = std::memory_order_relaxed) {
  return r64((void *)(base + offset), order);
}

inline void w64(uint64_t base, uint64_t offset, uint64_t val,
                std::memory_order order = std::memory_order_relaxed) {
  w64((void *)(base + offset), val, order);
}

inline uint32_t r32(uint64_t base, uint64_t offset,
                    std::memory_order order = std::memory_order_relaxed) {
  return r32((void *)(base + offset), order);
}

inline void w32(uint64_t base, uint64_t offset, uint32_t val,
                std::memory_order order = std::memory_order_relaxed) {
  w32((void *)(base + offset), val, order);
}

inline uint16_t r16(uint64_t base, uint64_t offset,
                    std::memory_order order = std::memory_order_relaxed) {
  return r16((void *)(base + offset), order);
}

inline void w16(uint64_t base, uint64_t offset, uint16_t val,
                std::memory_order order = std::memory_order_relaxed) {
  w16((void *)(base + offset), val, order);
}

inline uint8_t r8(uint64_t base, uint64_t offset,
                  std::memory_order order = std::memory_order_relaxed) {
  return r8((void *)(base + offset), order);
}

inline void w8(uint64_t base, uint64_t offset, uint8_t val,
               std::memory_order order = std::memory_order_relaxed) {
  w8((void *)(base + offset), val, order);
}

}  // namespace mmio