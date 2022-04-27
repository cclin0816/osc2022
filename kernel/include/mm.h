#pragma once

#include <config.h>
#include <conf.h>

namespace mm {

// // trans.cpp

uint64_t kn_to_phy(uint64_t addr);
uint64_t phy_to_kn(uint64_t addr);
uint64_t bus_to_kn(uint64_t addr);

// // vm.cpp

void init();
void init_vm();

// // alloc.cpp

void *kmalloc(uint64_t size);
void *kzalloc(uint64_t size);
void kfree(void *ptr);


}  // namespace mm