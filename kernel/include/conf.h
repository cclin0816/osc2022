#pragma once

// c header for kernel configuration

// 4KB page size
#define PAGE_SZ 0x1000UL

// 16KB kernel stack
#define KN_STACK_SZ (4UL * PAGE_SZ)

// 32KB user stack
#define USR_STACK_SZ (8UL * PAGE_SZ)


#define MAX_RSRV_MEM 48UL

#define MAX_USABLE_MEM 16UL

#define MAX_NORM_MEM 32UL

#define MAX_DEV_MEM 32UL

#define MAX_BUDDY_ORDER 16UL

#define IDX_FAIL 0xFFFFFFFFFFFFFFFFUL

// replacing nullptr, since sometime (void *)0 is valid
// pointer should initialize to nullptr still 
#define PTR_FAIL ((void *)0xFFFFFFFFFFFFFFFFUL)

// 40bit physical address space
#define PHY_ADDR_SZ 40UL
// 48bit high virtual address space
#define HIGH_ADDR_SZ 48UL
// 48bit low virtual address space
#define LOW_ADDR_SZ 48UL

#define FULL_MASK 0xFFFFFFFFFFFFFFFFUL

#define ZERO_MASK 0x0UL
