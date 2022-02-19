#include <bsl/align.h>
#include <bsl/cstring.h>
#include <bsl/endian.h>
#include <config.h>
#include <peripheral/uart.h>
#include <fdt/walk.h>

namespace fdt {

struct header_t {
  bu32_t magic;
  bu32_t totalsize;
  bu32_t off_dt_struct;
  bu32_t off_dt_strings;
  bu32_t off_mem_rsvmap;
  bu32_t version;
  bu32_t last_comp_version;
  bu32_t boot_cpuid_phys;
  bu32_t size_dt_strings;
  bu32_t size_dt_struct;
};

struct reserve_entry_t {
  bu64_t address;
  bu64_t size;
};

struct begin_node_t {
  bu32_t tag;
  // filler at least 4 bytes
  char name[4];
};

struct prop_t {
  bu32_t tag;
  bu32_t data_len;
  bu32_t name_off;
  // filler
  char data[1];
};

constexpr uint32_t fdt_magic = 0xd00dfeed;
constexpr uint32_t tag_begin_node = 0x1;
constexpr uint32_t tag_end_node = 0x2;
constexpr uint32_t tag_prop = 0x3;
constexpr uint32_t tag_nop = 0x4;
constexpr uint32_t tag_end = 0x9;

/**
 * @brief Walk the flatten device tree.
 * @param fdt_ptr pointer to the flatten device tree
 * @return true is succeed, false otherwise
 */
bool walk(void *const fdt_ptr) {
  auto *hdr = (header_t *)fdt_ptr;

  if (hdr->magic != fdt_magic) {  // not a fdt
    return false;
  }

  auto *dt_struct = (bu32_t *)((uint8_t *)fdt_ptr + hdr->off_dt_struct);
  // auto *dt_strings = (char *)((uint8_t *)fdt_ptr + hdr->off_dt_strings);
  // auto *mem_rsvmap =
  //     (reserve_entry_t *)((uint8_t *)fdt_ptr + hdr->off_mem_rsvmap);

  uint64_t depth = 0;
  bool fdt_end = false;

  while (!fdt_end) {
    uint32_t tag = *dt_struct;
    switch (tag) {
      case tag_begin_node: {
        auto *node = (begin_node_t *)(dt_struct);
        uint32_t name_len = bsl::p2align_up(bsl::strlen(node->name) + 1, 4);
        dt_struct = (bu32_t *)(&(node->name[name_len]));
        depth++;
        uart0::send("++\n");
        break;
      }
      case tag_end_node: {
        depth--;
        uart0::send("--\n");
      }
        [[fallthrough]];
      case tag_nop: {
        dt_struct++;
        break;
      }
      case tag_prop: {
        auto *prop = (prop_t *)(dt_struct);
        dt_struct =
            (bu32_t *)(&(prop->data[bsl::p2align_up((uint32_t)(prop->data_len), 4)]));
        break;
      }
      case tag_end: {
        fdt_end = true;
        break;
      }
      default: {
        // error tag
        return false;
      }
    }
  }
  return true;
}

}  // namespace fdt