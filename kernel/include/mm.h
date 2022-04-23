#pragma once

#include <config.h>

namespace mm {

/**
 * @brief add normal memory region to the memory map
 *
 * @param phy_addr physical address of the memory region
 * @param size size of the memory region in bytes
 * @param attr memory region attribute
 */
void add_norm_mem(uint64_t phy_addr, uint64_t size, uint64_t attr);
/**
 * @brief add device memory region to the memory map
 *
 * @param phy_addr physical address of the memory region
 * @param bus_addr bus address of the memory region
 * @param size size of the memory region in bytes
 * @param attr memory region attribute
 */
void add_dev_mem(uint64_t phy_addr, uint64_t bus_addr, uint64_t size,
                 uint64_t attr);
/**
 * @brief reserve memory region before buddy system initialized
 *
 * @param phy_addr physical address of the memory region
 * @param size size of the memory region in bytes
 * @param attr memory region attribute
 */
void early_rsrv_mem(uint64_t phy_addr, uint64_t size, uint64_t attr);
/**
 * @brief page allocation before buddy system initialized
 *
 * @param size size of the memory in bytes
 * @return void* address to the allocated memory
 */
void *early_alloc_page(uint64_t size);
/**
 * @brief page allocation after buddy system initialized
 *
 * @param size size of the memory in bytes
 * @return void* address to the allocated memory
 */
void *alloc_page(uint64_t size);
/**
 * @brief unified allocator
 *
 * @param size size of the memory in bytes
 * @return void* address of the allocated memory
 */
void *kmalloc(uint64_t size);
/**
 * @brief unified allocator, zero-filled
 *
 * @param size size of the memory in bytes
 * @return void* address of the allocated memory
 */
void *kzalloc(uint64_t size);
// unified free
void kfree(void *ptr);

void init();
void setup_vm();

uint64_t kn_to_phy(uint64_t addr);
uint64_t phy_to_kn(uint64_t addr);
uint64_t phy_to_bus(uint64_t addr);
uint64_t bus_to_phy(uint64_t addr);
uint64_t to_kn(uint64_t addr);
uint64_t to_phy(uint64_t addr);
uint64_t to_access(uint64_t addr);
void set_kaslr(uint64_t aslr);

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

constexpr uint64_t kn_mem =
    AF | valid;
    // RW_XX_tbl | UXN_tbl | UXN | AF | ISH | RW_XX | valid;

}  // namespace pa

inline constexpr bool has_attr(uint64_t attr, uint64_t attr_to_check) {
  return (attr & attr_to_check) == attr_to_check;
}

}  // namespace mm