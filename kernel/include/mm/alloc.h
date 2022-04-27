#pragma once

#include <mm.h>

namespace mm {

void add_usable_mem(uint64_t phy_addr, uint64_t size, uint64_t attr);
void add_rsrv_mem(uint64_t phy_addr, uint64_t size, uint64_t attr);

void init_early_alloc();
void *early_malloc(uint64_t size);
void *early_zalloc(uint64_t size);

void *alloc_page(uint64_t size);
void free_page(uint64_t pf_idx);

}  // namespace mm