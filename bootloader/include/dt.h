#pragma once

#include <bsl/pair.h>
#include <config.h>

namespace dt {

void init(void *fdt_ptr);
void *get_node(const char *name);
bsl::pair_t<void *, uint32_t> get_prop(void *node, const char *name);

uint32_t get_address_cells(void *node);
uint32_t get_size_cells(void *node);

void send_dt();

}  // namespace dt