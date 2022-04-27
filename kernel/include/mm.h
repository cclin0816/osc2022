#pragma once

#include <config.h>

namespace mm {

// trans.cpp

uint64_t kn_to_phy(uint64_t addr);
// inline constexpr void *kn_to_phy
uint64_t phy_to_kn(uint64_t addr);
uint64_t phy_to_bus(uint64_t addr);
uint64_t bus_to_phy(uint64_t addr);
uint64_t bus_to_kn(uint64_t addr);

// scan.cpp

void scan_mem_info();

// vm.cpp

void init_vm();
void init_vm_cleanup();

// alloc.cpp

void init_alloc();
void *kmalloc(uint64_t size);
void *kzalloc(uint64_t size);
void kfree(void *ptr);


}  // namespace mm