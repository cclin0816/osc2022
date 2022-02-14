#makefiles depency
MAKEFILE_DEP = $(abspath $(MAKEFILE_LIST))

# make targets:
COMMON_DIR = $(abspath ./common)
KN_DIR = kernel
KN_TARGET = kernel
BL_DIR = bootloader
BL_TARGET = bootloader
USR_DIR = user
IDIR = include
SDIR = src
BDIR = build
DBGDIR = debug
INC_DIR = -I $(COMMON_DIR)/$(IDIR) -I $(IDIR)
RSRC_DIR = resource
DTB = $(RSRC_DIR)/bcm2710-rpi-3-b-plus.dtb
INITRAMFS = $(RSRC_DIR)/initramfs.cpio

# tool chain
CC = clang
CXX = clang++
LD = ld.lld
ASM = clang
OBJCOPY = llvm-objcopy-13
OBJDUMP = llvm-objdump-13
# GDB = gdb-multiarch --se=kernel8.elf -ex 'gef' -ex 'target remote localhost:1234'
QEMU = qemu-system-aarch64 -M raspi3 -dtb $(DTB) -initrd $(INITRAMFS) -serial null -serial pty

# compiler flags
# using linux-gnu for libc++ header support, none-eabi is better if you don't need libc++
TARGET_ARCH = -target aarch64-none-linux-gnu
TARGET_CPU = -mcpu=cortex-a53
OPTIMIZE = -O3
WARNINGS = -Wall -Wextra -Wpedantic
BAREMETAL = -ffreestanding 
NOEXCEP = -fno-exceptions -fno-unwind-tables -fno-rtti
# LTO = -flto=thin
CSTANDARD = -std=c11
CXXSTANDARD = -std=c++20
CFLAGS = $(CSTANDARD) $(TARGET_ARCH) $(TARGET_CPU) 
CFLAGS += $(WARNINGS) $(BAREMETAL) -fpie
CXXFLAGS = $(CXXSTANDARD) $(TARGET_ARCH) $(TARGET_CPU) 
CXXFLAGS += $(WARNINGS) $(BAREMETAL) $(NOEXCEP) -fpie
LDFLAGS = --gc-sections
ASMFLAGS = $(TARGET_ARCH) $(TARGET_CPU) $(WARNINGS) -fpie
DBGFLAGS = -ggdb3 -Og


export CC CXX LD ASM OBJCOPY OBJDUMP CFLAGS CXXFLAGS LDFLAGS ASMFLAGS OPTIMIZE LTO
export DBGFLAGS COMMON_DIR IDIR SDIR BDIR DBGDIR BL_TARGET KN_TARGET MAKEFILE_DEP INC_DIR

all: common_all bootloader_all kernel_all user_all

common_all:
	@echo $(MAKEFILE_LIST)
	@echo "Make Common All"
	@make --no-print-directory -C $(COMMON_DIR)

bootloader_all: common_all
	@echo "Make Bootloader All"
	@make --no-print-directory -C $(BL_DIR)

kernel_all: common_all
	@echo "Make Kernel All"
	@make --no-print-directory -C $(KN_DIR)

user_all: common_all
	@echo "Make User All"
	@make --no-print-directory -C $(USR_DIR)

debug: common_debug bootloader_debug kernel_debug user_debug

common_debug:
	@echo "Make Common Debug"
	@make --no-print-directory -C $(COMMON_DIR) debug

bootloader_debug: common_debug
	@echo "Make Bootloader Debug"
	@make --no-print-directory -C $(BL_DIR) debug

kernel_debug: common_debug
	@echo "Make Kernel Debug"
	@make --no-print-directory -C $(KN_DIR) debug

user_debug: common_debug
	@echo "Make User Debug"
	@make --no-print-directory -C $(USR_DIR) debug

clean:
	@echo "Cleaning"
	@make --no-print-directory -C $(COMMON_DIR) clean
	@make --no-print-directory -C $(BL_DIR) clean
	@make --no-print-directory -C $(KN_DIR) clean
	@make --no-print-directory -C $(USR_DIR) clean

qemu: all
	@echo "Launch QEMU"
	$(QEMU) -kernel $(BL_DIR)/$(BDIR)/$(BL_TARGET).img

qemu_debug: debug
	@echo "Launch QEMU Debug"
	$(QEMU) -kernel $(BL_DIR)/$(DBGDIR)/$(BL_TARGET).img -S -s
