#pragma once

#include <mm.h>

namespace mm {

void add_norm_mem(uint64_t phy_addr, uint64_t size, uint64_t attr);
void add_dev_mem(uint64_t phy_addr, uint64_t bus_addr, uint64_t size,
                 uint64_t attr);

// page table attributes
namespace pa {

constexpr uint64_t NS_tbl = 0x1UL << 63U;
constexpr uint64_t RW_XX_tbl = 0x1UL << 61U;
constexpr uint64_t RO_RO_tbl = 0x2UL << 61U;
constexpr uint64_t RO_XX_tbl = 0x3UL << 61U;
constexpr uint64_t UXN_tbl = 0x1UL << 60U;
constexpr uint64_t PXN_tbl = 0x1UL << 59U;
constexpr uint64_t UXN = 0x1UL << 54U;
constexpr uint64_t PXN = 0x1UL << 53U;
constexpr uint64_t cont = 0x1UL << 52U;
constexpr uint64_t nG = 0x1UL << 11U;
constexpr uint64_t AF = 0x1UL << 10U;
constexpr uint64_t NSH = 0x0UL << 8U;
constexpr uint64_t OSH = 0x2UL << 8U;
constexpr uint64_t ISH = 0x3UL << 8U;
constexpr uint64_t RW_XX = 0x0UL << 6U;
constexpr uint64_t RW_RW = 0x1UL << 6U;
constexpr uint64_t RO_XX = 0x2UL << 6U;
constexpr uint64_t RO_RO = 0x3UL << 6U;
constexpr uint64_t NS = 0x1UL << 5U;

constexpr uint64_t DEV_nGnRnE = 0x0UL << 2U;
constexpr uint64_t NORM_nocache = 0x1UL << 2U;

constexpr uint64_t valid = 0x1UL;

constexpr uint64_t kn_mem = RW_XX_tbl | UXN_tbl | UXN | AF | RW_XX | valid;
constexpr uint64_t usr_mem = AF | RW_RW | valid;

}  // namespace pa

}  // namespace mm