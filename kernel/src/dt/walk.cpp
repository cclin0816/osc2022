#include <bsl/cassert.h>
#include <bsl/endian.h>
#include <dt.h>
#include <mmio.h>

namespace dt {

// struct header_t {
//   bu32_t magic;
//   bu32_t totalsize;
//   bu32_t off_dt_struct;
//   bu32_t off_dt_strings;
//   bu32_t off_mem_rsvmap;
//   bu32_t version;
//   bu32_t last_comp_version;
//   bu32_t boot_cpuid_phys;
//   bu32_t size_dt_strings;
//   bu32_t size_dt_struct;
// };

// struct reserve_entry_t {
//   bu64_t address;
//   bu64_t size;
// };

// struct begin_node_t {
//   bu32_t tag;
//   // filler at least 4 bytes
//   char name[4];
// };

// struct prop_t {
//   bu32_t tag;
//   bu32_t data_len;
//   bu32_t name_off;
//   // filler
//   char data[1];
// };

// constexpr uint32_t fdt_magic = 0xd00dfeed;
// constexpr uint32_t tag_begin_node = 0x1;
// constexpr uint32_t tag_end_node = 0x2;
// constexpr uint32_t tag_prop = 0x3;
// constexpr uint32_t tag_nop = 0x4;
// constexpr uint32_t tag_end = 0x9;

// header_t *_fdt_ptr = nullptr;

// void init_dt(void *fdt_ptr) {
//   // TODO
//   _fdt_ptr = (header_t *)fdt_ptr;
//   assert(_fdt_ptr->magic == fdt_magic);
// }

// dt_hdl get_hdl(bsl::sv_t name) {
//   // TODO
//   auto hdl = dt_hdl{};
//   return hdl;
// }

// dt_hdl get_subnode(dt_hdl hdl, bsl::sv_t name) {
//   // TODO
//   auto ret = dt_hdl{};
//   return ret;
// }

// uint32_t get_address_cells(dt_hdl hdl) {
//   using namespace bsl::svl;
//   auto *ac_data = dt::get_subnode(hdl, "#address-cells"sv).prop_data().first;
//   if (ac_data != nullptr) {
//     return *(bu32_t *)ac_data;
//   } else {
//     return 2;
//   }
// }

// uint32_t get_size_cells(dt_hdl hdl) {
//   using namespace bsl::svl;
//   auto *sc_data = dt::get_subnode(hdl, "#size-cells"sv).prop_data().first;
//   if (sc_data != nullptr) {
//     return *(bu32_t *)sc_data;
//   } else {
//     return 1;
//   }
// }

// bsl::pair_t<void *, uint32_t> dt_hdl::prop_data() {
//   // TODO
//   return bsl::pair_t<void *, uint32_t>{nullptr, 0};
// }

// uint64_t read_prop(void *data, uint32_t size) {
//   uint64_t ret = bu32_t(mmio::r32(data));
//   if (size > 1) {
//     ret = ret << 32 | bu32_t(mmio::r32(((uint32_t *)data) + 1));
//   }
//   return ret;
// }

}  // namespace dt
