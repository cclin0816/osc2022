#include <bsl/align.h>
#include <bsl/array.h>
#include <bsl/cassert.h>
#include <bsl/cdll.h>
#include <bsl/cmath.h>
#include <bsl/cstring.h>
#include <bsl/static_vec.h>
#include <mm/alloc.h>
#include <mm/trans.h>
#include <mm/types.h>
#include <sync.h>

#include <algorithm>

namespace {

bool buddy_init = false;
uint64_t early_alloc_head = 0;
bsl::static_vec<mm::mem_region_t, MAX_RSRV_MEM> rsrv_mem_regs;
bsl::static_vec<mm::mem_region_t, MAX_USABLE_MEM> usable_mem_regs;

// buddy system

bsl::array_t<bsl::cdll_t, MAX_BUDDY_ORDER> bd_fls;
mm::page_t *bd_pf = nullptr;
uint64_t bd_pf_sz = 0;

uint64_t ptr_to_pf_idx(void *ptr) {
  auto phy_addr = mm::kn_to_phy((uint64_t)ptr);
  auto pf_idx = phy_addr / PAGE_SZ;
  if (pf_idx >= bd_pf_sz) [[unlikely]] {
    return IDX_FAIL;
  } else {
    return pf_idx;
  }
}

void *pf_idx_to_ptr(uint64_t idx) {
  auto phy_addr = idx * PAGE_SZ;
  return (void *)mm::phy_to_kn(phy_addr);
}

uint64_t buddy_idx(uint64_t idx, uint64_t order) {
  auto bd_idx = idx ^ (1UL << order);
  if (bd_idx >= bd_pf_sz) [[unlikely]] {
    return IDX_FAIL;
  } else {
    return bd_idx;
  }
}

constexpr uint64_t sz_to_ord(uint64_t sz) {
  sz /= PAGE_SZ;
  return (uint64_t)bsl::log2_ceil(sz);
}

namespace pf {

constexpr uint64_t BUDDY_BODY = 0x0;
constexpr uint64_t BUDDY_USE = 0x1;
constexpr uint64_t BUDDY_FREE = 0x2;
constexpr uint64_t RSRV_USE = 0x4;
constexpr uint64_t SLAB_USE = 0x8;

}  // namespace pf

}  // namespace

namespace mm {

void *kmalloc(uint64_t size) {
  if (size == 0) [[unlikely]] {
    return PTR_FAIL;
  }
  if (size >= PAGE_SZ) {
    size = bsl::p2align_up(size, PAGE_SZ);
    if (buddy_init) [[likely]] {
      return alloc_page(size);
    } else {
      return early_malloc(size);
    }
  }
  // temporary solution
  return alloc_page(bsl::p2align_up(size, PAGE_SZ));
}

void *kzalloc(uint64_t size) {
  auto *ptr = kmalloc(size);
  if (ptr != PTR_FAIL) {
    bsl::memset(ptr, 0, size);
  }
  return ptr;
}

void kfree([[maybe_unused]] void *ptr) {
  auto idx = ptr_to_pf_idx(ptr);
  if (idx == IDX_FAIL) [[unlikely]] {
    panic();
    return;
  }
  auto flag = bd_pf[idx].flag;
  switch (flag) {
    case pf::BUDDY_USE:
      free_page(idx);
      break;
    case pf::RSRV_USE:
      todo();
      break;
    case pf::SLAB_USE:
      todo();
      break;
    default:
      panic();
      break;
  }
}

void *alloc_page(uint64_t size) {
  auto ord = sz_to_ord(size);
  sync::cpu_lck lck;

  // find free chunk
  auto find_ord = ord;
  while (true) {
    if (find_ord >= MAX_BUDDY_ORDER) [[unlikely]] {
      return PTR_FAIL;
    }
    if (!bd_fls[find_ord].empty()) {
      break;
    }
    find_ord++;
  }
  auto *chunk = bd_fls[find_ord].pop_back();
  auto chunk_idx = ptr_to_pf_idx(chunk);

  // release remain chunks
  while (find_ord > ord) {
    find_ord--;
    auto bd_idx = buddy_idx(chunk_idx, find_ord);
    bd_pf[bd_idx].flag = pf::BUDDY_FREE;
    bd_pf[bd_idx].order = find_ord;
    bd_fls[find_ord].push_back((bsl::cdll_t *)pf_idx_to_ptr(bd_idx));
  }

  // set chunk flag
  bd_pf[chunk_idx].flag = pf::BUDDY_USE;
  bd_pf[chunk_idx].order = ord;
  return chunk;
}

void free_page(uint64_t pf_idx) {
  sync::cpu_lck lck;
  auto order = bd_pf[pf_idx].order;
  bd_pf[pf_idx].flag = pf::BUDDY_BODY;
  bd_pf[pf_idx].order = 0;
  auto head_idx = pf_idx;
  while (order < MAX_BUDDY_ORDER - 1UL) {
    auto bd_idx = buddy_idx(head_idx, order);
    if (bd_idx < 0 || bd_pf[bd_idx].order != order ||
        bd_pf[bd_idx].flag != pf::BUDDY_FREE) {
      break;
    }
    auto *bd_ptr = (bsl::cdll_t *)pf_idx_to_ptr((uint64_t)bd_idx);
    bd_ptr->remove();
    bd_pf[bd_idx].flag = pf::BUDDY_BODY;
    bd_pf[bd_idx].order = 0;
    head_idx = std::min(head_idx, (uint64_t)bd_idx);
    ++order;
  }
  bd_pf[head_idx].flag = pf::BUDDY_FREE;
  bd_pf[head_idx].order = order;
  auto *head_ptr = (bsl::cdll_t *)pf_idx_to_ptr((uint64_t)head_idx);
  bd_fls[order].push_back(head_ptr);
}

void init_alloc() {
  std::sort(usable_mem_regs.begin(), usable_mem_regs.end());
  assert(!mm::collide_any(usable_mem_regs.begin(), usable_mem_regs.end()));

  for (auto &free_list : bd_fls) {
    free_list.init();
  }

  auto pf_ed = 0UL;
  for (auto &usable_mem : usable_mem_regs) {
    if (pf_ed < usable_mem.addr1) {
      mm::add_rsrv_mem(pf_ed, usable_mem.addr1 - pf_ed, 0);
    }
    pf_ed = usable_mem.addr1 + usable_mem.size;
  }
  bd_pf_sz = pf_ed / PAGE_SZ;
  bd_pf = (mm::page_t *)phy_to_kn((uint64_t)early_zalloc(
      bsl::p2align_up(bd_pf_sz * sizeof(page_t), PAGE_SZ)));

  std::sort(rsrv_mem_regs.begin(), rsrv_mem_regs.end());
  assert(!mm::collide_any(rsrv_mem_regs.begin(), rsrv_mem_regs.end()));

  // temporary solution
  for (auto res_mem : rsrv_mem_regs) {
    auto st_idx = res_mem.addr1 / PAGE_SZ;
    auto ed_idx = (res_mem.addr1 + res_mem.size) / PAGE_SZ;
    for (auto itr = st_idx; itr < ed_idx; ++itr) {
      bd_pf[itr].flag = pf::RSRV_USE;
    }
  }
  for (auto itr = 0UL; itr < bd_pf_sz; ++itr) {
    if (bd_pf[itr].flag == pf::BUDDY_BODY) {
      free_page(itr);
    }
  }

  buddy_init = true;
}

void add_usable_mem(uint64_t phy_addr, uint64_t size, uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ) &&
         usable_mem_regs.size() < MAX_USABLE_MEM);
  usable_mem_regs.push_back(phy_addr, 0UL, size, attr);
}

void add_rsrv_mem(uint64_t phy_addr, uint64_t size, uint64_t attr) {
  assert(bsl::is_p2aligned(phy_addr, PAGE_SZ) &&
         bsl::is_p2aligned(size, PAGE_SZ) &&
         rsrv_mem_regs.size() < MAX_RSRV_MEM);
  rsrv_mem_regs.push_back(phy_addr, 0UL, size, attr);
}

void init_early_alloc() {
  // temporary solution
  auto &last = rsrv_mem_regs.back();
  early_alloc_head = last.addr1 + last.size;
}

void *early_malloc(uint64_t size) {
  auto *ptr = (void *)early_alloc_head;
  add_rsrv_mem(early_alloc_head, size, 0);
  early_alloc_head += size;
  return ptr;
}

void *early_zalloc(uint64_t size) {
  auto *ptr = early_malloc(size);
  if (ptr != PTR_FAIL) {
    auto acc_ptr = (void *)mm::to_access((uint64_t)ptr);
    bsl::memset(acc_ptr, 0, size);
  }
  return ptr;
}

}  // namespace mm