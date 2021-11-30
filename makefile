KN_DIR = kernel
BL_DIR = bootloader
USR_DIR = user

CC = clang
CXX = clang++
LD = ld.lld
OBJCOPY = llvm-objcopy-13
OBJDUMP = llvm-objdump-13
# GDB = gdb-multiarch --se=kernel8.elf -ex 'gef' -ex 'target remote localhost:1234'

TARGET_ARCH = aarch64-arm-none-eabi
TARGET_CPU = cortex-a53
OPTIMIZE = -O3
WARNINGS = -Wall -Wextra -Wpedantic -Werror
BAREMETAL = -ffreestanding 
CXX_NOEXCEP = -fno-exceptions -fno-unwind-tables -fno-rtti

CFLAGS = -std=c11 -target $TARGET_ARCH -mcpu=$TARGET_CPU $WARNINGS $BAREMETAL
CXXFLAGS = -std=c++17 -target $TARGET_ARCH -mcpu=$TARGET_CPU $WARNINGS $BAREMETAL $CXX_NOEXCEP
LDFLAGS = --gc-sections -nostdlib -fpie
DBGFLAGS = -ggdb3 -Og

export CC CXX LD CFLAGS CXXFLAGS LDFLAGS OPTIMIZE DBGFLAGS

.PHONY: all clean

all:
	@echo "Building kernel"
	@make -C $(KN_DIR)
	@echo "Building bootloader"
	@make -C $(BL_DIR)
	@echo "Building user"
	@make -C $(USR_DIR)

clean:
	@echo "Cleaning kernel"
	@make -C $(KN_DIR) clean
	@echo "Cleaning bootloader"
	@make -C $(BL_DIR) clean
	@echo "Cleaning user"
	@make -C $(USR_DIR) clean


