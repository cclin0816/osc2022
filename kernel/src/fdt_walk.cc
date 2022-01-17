#include <align.h>
#include <endian.h>

#include <bsl/cstring>
#include <cstdint>

struct fdt_header {
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

struct fdt_reserve_entry {
  bu64_t address;
  bu64_t size;
};

struct fdt_begin_node {
  bu32_t tag;
  // filler at least 4 bytes
  char name[4];
};

struct fdt_prop {
  bu32_t tag;
  bu32_t data_len;
  bu32_t name_off;
  // filler
  char data[0];
};

constexpr uint32_t fdt_magic = 0xd00dfeed;
constexpr uint32_t tag_begin_node = 0x1;
constexpr uint32_t tag_end_node = 0x2;
constexpr uint32_t tag_prop = 0x3;
constexpr uint32_t tag_nop = 0x4;
constexpr uint32_t tag_end = 0x9;

// true if walk succeeded
bool fdt_walk(void *const _hdr) {
  auto *hdr = reinterpret_cast<fdt_header *>(_hdr);
  if (static_cast<uint32_t>(hdr->magic) != fdt_magic) {  // not a fdt
    return false;
  }

  auto *dt_struct =
      reinterpret_cast<bu32_t *>(reinterpret_cast<uint8_t *>(_hdr) +
                                 static_cast<uint32_t>(hdr->off_dt_struct));
  auto *dt_strings =
      reinterpret_cast<char *>(reinterpret_cast<uint8_t *>(_hdr) +
                               static_cast<uint32_t>(hdr->off_dt_strings));
  auto *mem_rsvmap = reinterpret_cast<fdt_reserve_entry *>(
      reinterpret_cast<uint8_t *>(_hdr) +
      static_cast<uint32_t>(hdr->off_mem_rsvmap));

  uint64_t depth = 0;
  bool fdt_end = false;

  while (!fdt_end) {
    uint32_t tag = *dt_struct;
    switch (tag) {
      case tag_begin_node: {
        auto *node = reinterpret_cast<fdt_begin_node *>(dt_struct);
        uint32_t name_len = bsl::align_up(bsl::strlen(node->name) + 1, 4);
        dt_struct = reinterpret_cast<bu32_t *>(&(node->name[name_len]));
        depth++;
        break;
      }
      case tag_end_node: {
        depth--;
      }
        [[fallthrough]];
      case tag_nop: {
        dt_struct++;
        break;
      }
      case tag_prop: {
        auto *prop = reinterpret_cast<fdt_prop *>(dt_struct);
        dt_struct = reinterpret_cast<bu32_t *>(
            &(prop->data[bsl::align_up(prop->data_len, 4)]));
        break;
      }
      case tag_end: {
        fdt_end = true;
        break;
      }
      default: {  // error tag
        return false;
      }
    }
  }
  return true;
}
