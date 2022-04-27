#pragma once

#include <bsl/algo.h>
#include <mm.h>

#include <compare>

namespace mm {

struct mem_region_t {
  uint64_t addr1 = 0;
  uint64_t addr2 = 0;
  uint64_t size = 0;
  uint64_t attr = 0;
  mem_region_t() = default;
  mem_region_t(uint64_t addr1, uint64_t addr2, uint64_t size, uint64_t attr)
      : addr1(addr1), addr2(addr2), size(size), attr(attr) {}
  auto operator<=>(const mem_region_t &rhs) const {
    return addr1 <=> rhs.addr1;
  }
  auto operator<=>(uint64_t rhs) const { return addr1 <=> rhs; }
  [[nodiscard]] auto collide(const mem_region_t &rhs) const {
    return bsl::collide(addr1, size, rhs.addr1, rhs.size);
  }
};

bool collide_any(const mem_region_t *begin, const mem_region_t *end);

struct page_t {
  uint64_t flag;
  union {
    uint64_t order;
    void *slab;
  };
};

}  // namespace mm