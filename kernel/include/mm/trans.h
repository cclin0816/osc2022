#pragma once

#include <mm.h>

namespace mm {

uint64_t to_kn(uint64_t addr);
uint64_t to_phy(uint64_t addr);
uint64_t to_access(uint64_t addr);
void set_kaslr(uint64_t aslr);
void set_vm_enable();

}