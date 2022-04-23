#pragma once

#include <bsl/string_view.h>
#include <bsl/pair.h>
#include <config.h>

namespace dt {

// class dt_hdl {
//  public:
//   union {
//     struct {
//       void *node;
//     } flat;
//     struct {
//     } tree;
//   } data;
//   uint8_t depth;
//   uint8_t tag;

//   dt_hdl next();
//   bsl::sv_t name();
//   bsl::pair_t<void *, uint32_t> prop_data();
// };

void init(void *fdt_ptr);
// void inflate_dt();

// bsl::sv_t alias_lookup(bsl::sv_t alias);
// bsl::sv_t symbol_lookup(bsl::sv_t symbol);

// dt_hdl get_hdl(bsl::sv_t name);
// dt_hdl get_subnode(dt_hdl hdl, bsl::sv_t name);
// void itr_subnode(dt_hdl hdl, void (*cb)(dt_hdl), bool rec = false);

// uint32_t get_address_cells(dt_hdl hdl);
// uint32_t get_size_cells(dt_hdl hdl);

// uint64_t read_prop(void *data, uint32_t size);


}  // namespace dt