#include <bsl/align.h>
#include <bsl/array.h>
#include <bsl/cassert.h>
#include <bsl/cmath.h>
#include <bsl/static_vec.h>
#include <kn_conf.h>
#include <mm.h>

namespace {

struct norm_mem_t {
  uint64_t phys_addr = 0;
  uint64_t size = 0;
  uint64_t attr = 0;
  norm_mem_t() = default;
  norm_mem_t(uint64_t phys_addr, uint64_t size, uint64_t attr)
      : phys_addr(phys_addr), size(size), attr(attr) {}
};

struct dev_mem_t {
 public:
  uint64_t phys_addr = 0;
  uint64_t bus_addr = 0;
  uint64_t size = 0;
  uint64_t attr = 0;
  dev_mem_t() = default;
  dev_mem_t(uint64_t phys_addr, uint64_t bus_addr, uint64_t size, uint64_t attr)
      : phys_addr(phys_addr), bus_addr(bus_addr), size(size), attr(attr) {}
};

bsl::static_vec<norm_mem_t, MAX_NORM_MEM> norm_mem_layout;
bsl::static_vec<dev_mem_t, MAX_DEV_MEM> dev_mem_layout;

struct tbl_dscr_t {
  uint64_t type : 2 = 0;
  uint64_t : 10;
  uint64_t addr : 36 = 0;  // physical address of next level table
  uint64_t : 11;
  uint64_t PXN : 1 = 0;
  uint64_t UXN : 1 = 0;
  uint64_t AP : 2 = 0;
  uint64_t NS : 1 = 0;
  tbl_dscr_t() = default;
  void set_attr(uint64_t attr) {
    const uint64_t attr_mask = 31UL << 59U;
    auto *self_view = (uint64_t *)this;
    *self_view = (*self_view & ~attr_mask) | (attr & attr_mask);
  }
} PACKED;

struct blk_dscr_t {
  uint64_t type : 2 = 0;
  uint64_t Attr_indx : 3 = 0;
  uint64_t NS : 1 = 0;
  uint64_t AP : 2 = 0;
  uint64_t SH : 2 = 0;
  uint64_t AF : 1 = 0;
  uint64_t nG : 1 = 0;
  uint64_t addr : 36 = 0;  // physical address of the block
  uint64_t : 4;
  uint64_t cont : 1 = 0;
  uint64_t PXN : 1 = 0;
  uint64_t UXN : 1 = 0;
  uint64_t res_soft : 4 = 0;
  uint64_t : 5;
  blk_dscr_t() = default;
  void set_attr(uint64_t attr) {
    const uint64_t attr_mask = 127UL << 52U | 1023U << 2U;
    auto *self_view = (uint64_t *)this;
    *self_view = (*self_view & ~attr_mask) | (attr & attr_mask);
  }
} PACKED;

struct pg_tbl_entry_t {
  union {
    tbl_dscr_t tbl_dscr;
    blk_dscr_t blk_dscr;
    uint64_t val;
  };
  pg_tbl_entry_t() { val = 0; }
  pg_tbl_entry_t &operator=(const pg_tbl_entry_t &rhs) {
    if (this != &rhs) {
      val = rhs.val;
    }
    return *this;
  }
  [[nodiscard]] bool is_valid() const { return (tbl_dscr.type & 0x1UL) == 1UL; }
  [[nodiscard]] bool is_last(uint8_t depth) const {
    return !(tbl_dscr.type == 3UL && depth != 3U);
  }
};
static_assert(sizeof(pg_tbl_entry_t) == sizeof(uint64_t), "invalid size");

constexpr auto tbl_entry_count = PAGE_SZ / 8UL;
using pg_tbl_t = bsl::array_t<pg_tbl_entry_t, tbl_entry_count>;
static_assert(sizeof(pg_tbl_t) == PAGE_SZ, "invalid size");

pg_tbl_t *kn_pt_root = nullptr;

/**
 * @brief get shift amoount for translation depth, only 4K page implemented
 *
 * @param depth depth of the page table
 * @return shift amount in bits
 */
PURE inline constexpr uint32_t pt_shift(const uint8_t depth) {
  ASSUME(depth <= 3U);
  return 12U + (9U * (3U - depth));
}

/**
 * @brief get index of address for translation depth
 *
 * @param addr address
 * @param depth depth of the page table
 * @return index of address
 */
PURE inline constexpr uint32_t pt_idx(const uint64_t addr,
                                      const uint8_t depth) {
  return (addr >> pt_shift(depth)) & (tbl_entry_count - 1UL);
}

/**
 * @brief get block size for translation depth
 *
 * @param depth depth of the page table
 * @return block size in bytes
 */
PURE inline constexpr uint64_t pt_blk_sz(const uint8_t depth) {
  return 1UL << pt_shift(depth);
}

// physical address
inline uint64_t get_dscr_addr(pg_tbl_entry_t &entry) {
  return entry.tbl_dscr.addr << 12UL;
}

// physical address
inline void set_dscr_addr(pg_tbl_entry_t &entry, const uint64_t addr) {
  entry.tbl_dscr.addr = addr >> 12UL;
}

void set_part_pte(pg_tbl_entry_t &pte, uint64_t phys_addr, uint64_t vm_addr,
                  uint64_t size, uint64_t attr, uint8_t depth);

void set_full_pte(pg_tbl_entry_t &pte, uint64_t phys_addr, uint64_t vm_addr,
                  uint64_t size, uint64_t attr, uint8_t depth);

/**
 * @brief expand page table entry to next level
 *
 * @param pte page table entry, assume is_last
 * @param attr attributes for the new table descriptor
 * @param depth current page table entry depth, assume < 3
 */
void expand_pte(pg_tbl_entry_t &pte, uint64_t attr, uint8_t depth) {
  pg_tbl_t *new_pg_tbl;
  pg_tbl_entry_t new_pte;

  if (pte.is_valid()) {
    // block entry, expand mapping
    new_pg_tbl = (pg_tbl_t *)mm::kmalloc(PAGE_SZ);
    auto addr = get_dscr_addr(pte);
    const auto blk_sz = pt_blk_sz(depth + 1);
    // 3 if page, 1 if block
    ASSUME(depth < 3U && depth > 0U);
    const auto type = (depth == 2U ? 3U : 1U);
    for (auto &itr_pte : *new_pg_tbl) {
      // copy all attributes from old page table
      itr_pte = pte;
      // set type
      itr_pte.blk_dscr.type = type;
      // set address
      set_dscr_addr(itr_pte, addr);
      addr += blk_sz;
    }
  } else {
    // fault entry, create zero mapping
    new_pg_tbl = (pg_tbl_t *)mm::kzalloc(PAGE_SZ);
  }
  new_pte.tbl_dscr.set_attr(attr);
  new_pte.tbl_dscr.type = 3;
  set_dscr_addr(new_pte, mm::to_phy((uint64_t)new_pg_tbl));
  pte = new_pte;
}

void clear_pte(pg_tbl_entry_t &pte, uint8_t depth);

void clear_pg_tbl(pg_tbl_t &pg_tbl, uint8_t depth) {
  for (auto &itr_pte : pg_tbl) {
    clear_pte(itr_pte, depth);
  }
}

void clear_pte(pg_tbl_entry_t &pte, uint8_t depth) {
  if (!pte.is_last(depth)) {
    auto *next_lvl = (pg_tbl_t *)mm::to_access(get_dscr_addr(pte));
    clear_pg_tbl(*next_lvl, depth + 1);
    mm::kfree(next_lvl);
  }
  pte.val = 0UL;
}

/**
 * @brief set page table
 *
 * @param pt page table
 * @param phys_addr physical address, assuming page aligned
 * @param vm_addr virtual address, assuming page aligned
 * @param size size of memory to map, assuming page aligned
 * @param attr memory attribute
 * @param depth page table depth
 */
void set_pg_tbl(pg_tbl_t &pt, uint64_t phys_addr, const uint64_t vm_addr,
                const uint64_t size, const uint64_t attr, const uint8_t depth) {
  const auto blk_sz = pt_blk_sz(depth);
  auto st_vm_addr = bsl::p2align_down(vm_addr, blk_sz);
  auto ed_vm_addr = bsl::p2align_down(vm_addr + size, blk_sz);
  auto st_idx = pt_idx(st_vm_addr, depth);

  if (st_vm_addr == ed_vm_addr) {
    // partial single block
    set_part_pte(pt[st_idx], phys_addr, vm_addr, size, attr, depth);
    return;
  }

  // align start
  if (st_vm_addr != vm_addr) {
    auto seg_sz = blk_sz - (vm_addr - st_vm_addr);
    set_part_pte(pt[st_idx], phys_addr, vm_addr, seg_sz, attr, depth);
    st_vm_addr += blk_sz;
    phys_addr += seg_sz;
    st_idx++;
  }
  // loop
  for (; st_vm_addr < ed_vm_addr;
       st_vm_addr += blk_sz, phys_addr += blk_sz, st_idx++) {
    set_full_pte(pt[st_idx], phys_addr, st_vm_addr, blk_sz, attr, depth);
  }
  // align end
  if (ed_vm_addr != vm_addr + size) {
    auto seg_sz = vm_addr + size - ed_vm_addr;
    set_part_pte(pt[st_idx], phys_addr, ed_vm_addr, seg_sz, attr, depth);
  }
}

void set_part_pte(pg_tbl_entry_t &pte, uint64_t phys_addr, uint64_t vm_addr,
                  uint64_t size, uint64_t attr, uint8_t depth) {
  if (pte.is_last(depth)) {
    expand_pte(pte, attr, depth);
  }
  auto *next_lvl = (pg_tbl_t *)mm::to_access(get_dscr_addr(pte));
  set_pg_tbl(*next_lvl, phys_addr, vm_addr, size, attr, depth + 1);
}

void set_full_pte(pg_tbl_entry_t &pte, uint64_t phys_addr, uint64_t vm_addr,
                  uint64_t size, uint64_t attr, uint8_t depth) {
  if (depth == 0) {
    // no level 0 block descriptor
    set_part_pte(pte, phys_addr, vm_addr, size, attr, depth);
  }
  clear_pte(pte, depth);
  if (mm::has_attr(attr, mm::pa::valid)) {
    pg_tbl_entry_t new_pte;
    new_pte.blk_dscr.set_attr(attr);
    set_dscr_addr(new_pte, phys_addr);
    auto type = (depth < 3U ? 1UL : 3UL);
    new_pte.blk_dscr.type = type;
    pte = new_pte;
  }
}

}  // namespace

namespace mm {

void add_norm_mem(uint64_t phy_addr, uint64_t size, uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ));
  norm_mem_layout.push_back(phy_addr, size, attr);
}

void add_dev_mem(uint64_t phy_addr, uint64_t bus_addr, uint64_t size,
                 uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(bus_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ));
  dev_mem_layout.push_back(phy_addr, bus_addr, size, attr);
}

void setup_vm() {
  // kmalloc is physical address now
  kn_pt_root = (pg_tbl_t *)kzalloc(PAGE_SZ);
  for (auto &mem : norm_mem_layout) {
    set_pg_tbl(*kn_pt_root, mem.phys_addr, phy_to_kn(mem.phys_addr), mem.size,
               mem.attr, 0);
  }
  for (auto &mem : dev_mem_layout) {
    set_pg_tbl(*kn_pt_root, mem.phys_addr, phy_to_kn(mem.phys_addr), mem.size,
               mem.attr, 0);
  }

  // setup tcr_el1, E0PD1, HA, AS, IPS = 48, TG1 = 4K, T1SZ = 48, TG0 = 4K, T0SZ
  // = 48
  asm volatile("msr tcr_el1, %0" ::"r"((2UL << 30U) | (16UL << 16U) |
                                       (0UL << 14U) | (16UL << 0U)));
  // (1UL << 56U) | (1UL << 39U) | (1UL << 36U) | (5UL << 32U) | (2UL << 30U) |
  // (16UL << 16U) | (0UL << 14U) | (16UL << 0U)));

  // setup mair_el1, A0 = DEVICE nGnRnE, A1 = NORMAL non-cacheable
  asm volatile(
      "msr mair_el1, %0" ::"r"((0b00000000UL << 0U) | (0b01000100UL << 8U)));

  // setup ttbr1_el1, ttbr0_el1 = kn_pt_root
  asm volatile(
      "msr ttbr1_el1, %0\n"
      "msr ttbr0_el1, %0" ::"r"((uint64_t)kn_pt_root));

  // setup sctlr_el1, enable MMU
  asm volatile(
      "mrs x2, sctlr_el1\n"
      "orr x2 , x2, 1\n"
      "msr sctlr_el1, x2\n" ::
          : "x2");

  // asm volatile("msr sctlr_el1, %0" ::"r"((1UL << 0U)));
  asm volatile("isb");
}

}  // namespace mm