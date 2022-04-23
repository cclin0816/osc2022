#include <bsl/align.h>
#include <bsl/cstring.h>
#include <bsl/endian.h>
#include <config.h>
#include <dt.h>
#include <peripheral/uart.h>

namespace dt {

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

// constexpr uint32_t fdt_magic = 0xd00dfeed;
constexpr uint32_t tag_begin_node = 0x1;
constexpr uint32_t tag_end_node = 0x2;
constexpr uint32_t tag_prop = 0x3;
constexpr uint32_t tag_nop = 0x4;
constexpr uint32_t tag_end = 0x9;

header_t *fdt_ptr = nullptr;

void init(void *_fdt_ptr) { fdt_ptr = (header_t *)_fdt_ptr; }

void *get_node(const char *name) {
  auto *dt_struct = (bu32_t *)((uint8_t *)fdt_ptr + fdt_ptr->off_dt_struct);

  while (true) {
    uint32_t tag = *dt_struct;
    switch (tag) {
      case tag_begin_node: {
        auto *node = (begin_node_t *)(dt_struct);
        if (bsl::strcmp(node->name, name) == 0) {
          return node;
        }
        uint32_t name_len = (uint32_t)bsl::p2align_up(bsl::strlen(node->name) + 1, 4);
        dt_struct = (bu32_t *)(&(node->name[name_len]));
        break;
      }
      case tag_end_node:
        [[fallthrough]];
      case tag_nop: {
        dt_struct++;
        break;
      }
      case tag_prop: {
        auto *prop = (prop_t *)(dt_struct);
        dt_struct = (bu32_t *)(&(
            prop->data[bsl::p2align_up((uint32_t)(prop->data_len), 4)]));
        break;
      }
      case tag_end: {
        return nullptr;
      }
      default: {
        return nullptr;
      }
    }
  }
  // return nullptr;
}

bsl::pair_t<void *, uint32_t> get_prop(void *node, const char *name) {
  auto *dt_struct = (bu32_t *)node;
  auto *dt_strings = (char *)((uint8_t *)fdt_ptr + fdt_ptr->off_dt_strings);
  uint64_t depth = 0;

  while (true) {
    uint32_t tag = *dt_struct;
    switch (tag) {
      case tag_begin_node: {
        auto *node = (begin_node_t *)(dt_struct);
        uint32_t name_len = (uint32_t)bsl::p2align_up(bsl::strlen(node->name) + 1, 4);
        dt_struct = (bu32_t *)(&(node->name[name_len]));
        depth++;
        break;
      }
      case tag_end_node: {
        depth--;
        if (depth == 0) {
          return {nullptr, 0};
        }
      }
        [[fallthrough]];
      case tag_nop: {
        dt_struct++;
        break;
      }
      case tag_prop: {
        auto *prop = (prop_t *)(dt_struct);
        if (depth == 1 && bsl::strcmp(dt_strings + prop->name_off, name) == 0) {
          return {&(prop->data[0]), prop->data_len};
        }
        dt_struct = (bu32_t *)(&(
            prop->data[bsl::p2align_up((uint32_t)(prop->data_len), 4)]));
        break;
      }
      case tag_end: {
        return {nullptr, 0};
      }
      default: {
        return {nullptr, 0};
      }
    }
  }
}

uint32_t get_address_cells(void *node) {
  auto [prop_ptr, prop_len] = get_prop(node, "#address-cells");
  if (prop_ptr == nullptr) {
    return 2;
  }
  return *(bu32_t *)prop_ptr;
}

uint32_t get_size_cells(void *node) {
  auto [prop_ptr, prop_len] = get_prop(node, "#size-cells");
  if (prop_ptr == nullptr) {
    return 1;
  }
  return *(bu32_t *)prop_ptr;
}

void send_dt() {
  uart::bcm2835_aux_t serial;
  serial.write<uint64_t>((uint64_t)fdt_ptr);
  serial.write<uint32_t>(fdt_ptr->totalsize);
  for(uint32_t i = 0; i < fdt_ptr->totalsize; i += 256) {
    serial.write(((char *)fdt_ptr + i), 256);
    serial.recv();
  }
}

}  // namespace dt