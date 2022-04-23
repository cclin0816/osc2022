#pragma once

// 4KB page size
#define PAGE_SZ 0x1000UL

// 16KB kernel stack
#define KN_STACK_SZ (4UL * PAGE_SZ)

// 32KB user stack
#define USR_STACK_SZ (8UL * PAGE_SZ)


#define MAX_RSRV_MEM 64UL

#define MAX_NORM_MEM 32UL

#define MAX_DEV_MEM 32UL

// replacing nullptr, since sometime (void *)0 is valid
// pointer should initialize to nullptr still 
#define PTR_FAIL ((void *)0xFFFFFFFFFFFFFFFF)
