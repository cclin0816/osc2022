# make targets:
COMMON_DIR = common
KN_DIR = kernel
BL_DIR = bootloader
USR_DIR = user

# tool chain
CC = clang
CXX = clang++
LD = $(CC)
OBJCOPY = llvm-objcopy-13
OBJDUMP = llvm-objdump-13
# GDB = gdb-multiarch --se=kernel8.elf -ex 'gef' -ex 'target remote localhost:1234'


# compiler flags
# using linux-gnu for libc++ header support, none-eabi is better if you don't need libc++
TARGET_ARCH = aarch64-linux-gnu
TARGET_CPU = cortex-a53
OPTIMIZE = -O3
WARNINGS = -Wall -Wextra -Wpedantic
BAREMETAL = -ffreestanding 
NOEXCEP = -fno-exceptions -fno-unwind-tables -fno-rtti
LTO = -flto=thin
CSTANDARD = -std=c11
CXXSTANDARD = -std=c++20
CFLAGS = $(CSTANDARD) -target $(TARGET_ARCH) -mcpu=$(TARGET_CPU) $(WARNINGS) $(BAREMETAL) $(LTO)
CXXFLAGS = $(CXXSTANDARD) -target $(TARGET_ARCH) -mcpu=$(TARGET_CPU) $(WARNINGS) $(BAREMETAL) $(NOEXCEP) $(LTO)
LDFLAGS = --gc-sections -nostdlib -static-pie $(LTO)
DBGFLAGS = -ggdb3 -Og

# include paths 
INCLUDE = -I $(abspath $(COMMON_DIR)/include)

export CC CXX LD CFLAGS CXXFLAGS LDFLAGS OPTIMIZE DBGFLAGS INCLUDE

.PHONY: all clean

all:
	@echo "Building Common"
	@make -C $(COMMON_DIR)
	@echo "Building Kernel"
	@make -C $(KN_DIR)
	@echo "Building Bootloader"
	@make -C $(BL_DIR)
	@echo "Building User"
	@make -C $(USR_DIR)

clean:
	@echo "Cleaning Common"
	@make -C $(COMMON_DIR) clean
	@echo "Cleaning Kernel"
	@make -C $(KN_DIR) clean
	@echo "Cleaning Bootloader"
	@make -C $(BL_DIR) clean
	@echo "Cleaning User"

