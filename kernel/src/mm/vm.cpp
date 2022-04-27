// #include <bsl/algo.h>
#include <algorithm>
#include <bsl/align.h>
#include <bsl/cassert.h>
#include <bsl/static_vec.h>
#include <bsl/array.h>
#include <compiler.h>
#include <mm.h>
#include <mm/alloc.h>
#include <mm/trans.h>
#include <mm/types.h>
#include <mm/vm.h>


namespace {

bsl::static_vec<mm::mem_region_t, MAX_NORM_MEM> norm_mem_regs;
bsl::static_vec<mm::mem_region_t, MAX_DEV_MEM> dev_mem_regs;

struct tbl_dscr_t {
  uint64_t type : 2 = 0;
  uint64_t : 10;
  uint64_t addr : 36 = 0;
  uint64_t : 11;
  uint64_t PXN : 1 = 0;
  uint64_t UXN : 1 = 0;
  uint64_t AP : 2 = 0;
  uint64_t NS : 1 = 0;
  tbl_dscr_t() = default;
  void set_attr(uint64_t attr, uint64_t mask = FULL_MASK) {
    const uint64_t attr_mask = (31UL << 59U) & mask;
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
  uint64_t addr : 36 = 0;
  uint64_t : 4;
  uint64_t cont : 1 = 0;
  uint64_t PXN : 1 = 0;
  uint64_t UXN : 1 = 0;
  uint64_t res_soft : 4 = 0;
  uint64_t : 5;
  blk_dscr_t() = default;
  void set_attr(uint64_t attr, uint64_t mask = FULL_MASK) {
    const uint64_t attr_mask = (127UL << 52U | 1023U << 2U) & mask;
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

// only 4K page implemented
PURE inline constexpr uint32_t pt_shift(const uint8_t depth) {
  ASSUME(depth <= 3U);
  return 12U + (9U * (3U - depth));
}

PURE inline constexpr uint32_t pt_idx(const uint64_t addr,
                                      const uint8_t depth) {
  return (addr >> pt_shift(depth)) & (tbl_entry_count - 1UL);
}

PURE inline constexpr uint64_t pt_blk_sz(const uint8_t depth) {
  return 1UL << pt_shift(depth);
}

inline uint64_t get_dscr_addr(pg_tbl_entry_t &entry) {
  return entry.tbl_dscr.addr << 12UL;
}

inline void set_dscr_addr(pg_tbl_entry_t &entry, const uint64_t addr) {
  entry.tbl_dscr.addr = addr >> 12UL;
}

void set_part_pte(pg_tbl_entry_t &pte, uint64_t phy_addr_or_attr_msk,
                  uint64_t vm_addr, uint64_t size, uint64_t attr, uint8_t depth,
                  bool set_attr_only);

void set_full_pte(pg_tbl_entry_t &pte, uint64_t phy_addr_or_attr_msk,
                  uint64_t vm_addr, uint64_t size, uint64_t attr, uint8_t depth,
                  bool set_attr_only);

/**
 * @brief expand page table entry to next level
 *
 * @param pte page table entry, assume is_last
 * @param attr attributes for the new table descriptor
 * @param depth current page table entry depth, assume < 3
 */
void expand_pte(pg_tbl_entry_t &pte, uint64_t attr, uint64_t attr_mask,
                uint8_t depth) {
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
  new_pte.tbl_dscr.set_attr(attr, attr_mask);
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

void set_pg_tbl(pg_tbl_t &pt, uint64_t phy_addr_or_attr_msk,
                const uint64_t vm_addr, const uint64_t size,
                const uint64_t attr, const uint8_t depth,
                const bool set_attr_only) {
  const auto blk_sz = pt_blk_sz(depth);
  auto st_vm_addr = bsl::p2align_down(vm_addr, blk_sz);
  auto ed_vm_addr = bsl::p2align_down(vm_addr + size, blk_sz);
  auto st_idx = pt_idx(st_vm_addr, depth);

  if (st_vm_addr == ed_vm_addr) {
    // partial single block
    set_part_pte(pt[st_idx], phy_addr_or_attr_msk, vm_addr, size, attr, depth,
                 set_attr_only);
    return;
  }

  // align start
  if (st_vm_addr != vm_addr) {
    auto seg_sz = blk_sz - (vm_addr - st_vm_addr);
    set_part_pte(pt[st_idx], phy_addr_or_attr_msk, vm_addr, seg_sz, attr, depth,
                 set_attr_only);
    st_vm_addr += blk_sz;
    if (!set_attr_only) {
      phy_addr_or_attr_msk += seg_sz;
    }
    st_idx++;
  }
  // loop
  for (; st_vm_addr < ed_vm_addr;) {
    set_full_pte(pt[st_idx], phy_addr_or_attr_msk, st_vm_addr, blk_sz, attr,
                 depth, set_attr_only);
    st_vm_addr += blk_sz;
    if (!set_attr_only) {
      phy_addr_or_attr_msk += blk_sz;
    }
    st_idx++;
  }
  // align end
  if (ed_vm_addr != vm_addr + size) {
    auto seg_sz = vm_addr + size - ed_vm_addr;
    set_part_pte(pt[st_idx], phy_addr_or_attr_msk, ed_vm_addr, seg_sz, attr,
                 depth, set_attr_only);
  }
}

void set_part_pte(pg_tbl_entry_t &pte, uint64_t phy_addr_or_attr_msk,
                  uint64_t vm_addr, uint64_t size, uint64_t attr, uint8_t depth,
                  bool set_attr_only) {
  if (pte.is_last(depth)) {
    uint64_t attr_mask = set_attr_only ? phy_addr_or_attr_msk : FULL_MASK;
    expand_pte(pte, attr, attr_mask, depth);
  }
  auto *next_lvl = (pg_tbl_t *)mm::to_access(get_dscr_addr(pte));
  set_pg_tbl(*next_lvl, phy_addr_or_attr_msk, vm_addr, size, attr, depth + 1,
             set_attr_only);
}

void set_full_pte(pg_tbl_entry_t &pte, uint64_t phy_addr_or_attr_msk,
                  uint64_t vm_addr, uint64_t size, uint64_t attr, uint8_t depth,
                  bool set_attr_only) {
  if (depth == 0) {
    // no level 0 block descriptor
    set_part_pte(pte, phy_addr_or_attr_msk, vm_addr, size, attr, depth,
                 set_attr_only);
    return;
  }
  if (!set_attr_only) {
    clear_pte(pte, depth);
    if (bsl::has_attr(attr, mm::pa::valid)) {
      pg_tbl_entry_t new_pte;
      new_pte.blk_dscr.set_attr(attr);
      set_dscr_addr(new_pte, phy_addr_or_attr_msk);
      auto type = (depth < 3U ? 1UL : 3UL);
      new_pte.blk_dscr.type = type;
      pte = new_pte;
    }
  } else {
    if (pte.is_last(depth)) {
      pte.blk_dscr.set_attr(attr, phy_addr_or_attr_msk);
    } else {
      pte.tbl_dscr.set_attr(attr, phy_addr_or_attr_msk);
      set_part_pte(pte, phy_addr_or_attr_msk, vm_addr, size, attr, depth,
                   set_attr_only);
    }
  }
}

pg_tbl_t *global_fault;

}  // namespace

namespace mm {

void init() {
  init_early_alloc();
  kn_pt_root = (pg_tbl_t *)early_zalloc(PAGE_SZ);

  std::sort(norm_mem_regs.begin(), norm_mem_regs.end());
  std::sort(dev_mem_regs.begin(), dev_mem_regs.end());
  assert(!mm::collide_any(norm_mem_regs.begin(), norm_mem_regs.end()) &&
         !mm::collide_any(dev_mem_regs.begin(), dev_mem_regs.end()));

  for (auto &mem : norm_mem_regs) {
    set_pg_tbl(*kn_pt_root, mem.addr1, phy_to_kn(mem.addr1), mem.size, mem.attr,
               0, false);
  }
  for (auto &mem : dev_mem_regs) {
    set_pg_tbl(*kn_pt_root, mem.addr1, phy_to_kn(mem.addr1), mem.size, mem.attr,
               0, false);
  }
  // setup tcr_el1, E0PD1, HA, AS, IPS = 40, TG1 = 4K, T1SZ = 48, TG0 = 4K, T0SZ
  // = 48
  asm volatile("msr tcr_el1, %0" ::"r"(
      (1UL << 56U) | (1UL << 39U) | (1UL << 36U) | (2UL << 32U) | (2UL << 30U) |
      ((64UL - HIGH_ADDR_SZ) << 16U) | (0UL << 14U) |
      ((64UL - LOW_ADDR_SZ) << 0U)));

  // set mair_el1, A0 = DEVICE nGnRnE, A1 = NORMAL non-cacheable
  asm volatile(
      "msr mair_el1, %0" ::"r"((0b00000000UL << 0U) | (0b01000100UL << 8U)));

  // set ttbr1_el1, ttbr0_el1 = kn_pt_root
  asm volatile(
      "msr ttbr1_el1, %0\n"
      "msr ttbr0_el1, %0" ::"r"((uint64_t)kn_pt_root));

  // set sctlr_el1, SPAN, CP15BEN, SA0, SA, M
  asm volatile("msr sctlr_el1, %0" ::"r"(
      (1UL << 23U) | (1UL << 5U) | (1UL << 4U) | (1UL << 3U) | (1UL << 0U)));

  dsb(ish);
  isb();
  set_vm_enable();
}

void init_vm() {
  kn_pt_root = (pg_tbl_t *)phy_to_kn((uint64_t)kn_pt_root);
  auto fault_pg_tbl = (uint64_t)mm::early_zalloc(PAGE_SZ);
  global_fault = (pg_tbl_t *)phy_to_kn(fault_pg_tbl);

  // set ttbr0_el1 to fault_pg_tbl
  asm volatile("msr ttbr0_el1, %0" ::"r"(fault_pg_tbl));
  dsb(ish);
  tlb_flush(is);
  dsb(ish);
  isb();

  // swap dev_mem_regs addr1 addr2, so addr1 is bus addr
  for (auto &dev_mem : dev_mem_regs) {
    std::swap(dev_mem.addr1, dev_mem.addr2);
  }
  std::sort(dev_mem_regs.begin(), dev_mem_regs.end());
  assert(!mm::collide_any(dev_mem_regs.begin(), dev_mem_regs.end()));
}

void add_norm_mem(uint64_t phy_addr, uint64_t size, uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ) &&
         norm_mem_regs.size() < MAX_NORM_MEM);
  norm_mem_regs.push_back(phy_addr, 0UL, size, attr);
}

void add_dev_mem(uint64_t phy_addr, uint64_t bus_addr, uint64_t size,
                 uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(bus_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ) && dev_mem_regs.size() < MAX_DEV_MEM);
  dev_mem_regs.push_back(phy_addr, bus_addr, size, attr);
}

uint64_t bus_to_phy(uint64_t bus_addr) {
  auto lb =
      std::lower_bound(dev_mem_regs.begin(), dev_mem_regs.end(), bus_addr);
  if (lb == dev_mem_regs.end()) {
    lb--;
  } else if (*lb > bus_addr) {
    if (lb == dev_mem_regs.begin()) {
      return IDX_FAIL;
    }
    lb--;
  }
  if (lb->addr1 + lb->size <= bus_addr) {
    return IDX_FAIL;
  }
  return lb->addr2 + (bus_addr - lb->addr1);
}

}  // namespace mm