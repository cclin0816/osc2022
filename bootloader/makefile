# makefile dependency
MAKEFILE_DEP += $(MAKEFILE_LIST)

# header
HDRS = $(shell find $(IDIR) -type f)
HDRS += $(shell find $(COMMON_DIR)/$(IDIR) -type f)

# depenency
LINKER = linker.ld
OBJS = $(shell find $(SDIR) -type f | sed -e 's/\.[^.]*$$/.o/')
BUILD_OBJS = $(OBJS:$(SDIR)/%.o=$(BDIR)/%.o)
LIB_BUILD_OBJS = $(shell find $(COMMON_DIR)/$(BDIR) -type f -name '*.o')
DEBUG_OBJS = $(OBJS:$(SDIR)/%.o=$(DBGDIR)/%.o)
LIB_DEBUG_OBJS = $(shell find $(COMMON_DIR)/$(DBGDIR) -type f -name '*.o')

# only general register
CFLAGS += -mgeneral-regs-only
CXXFLAGS += -mgeneral-regs-only
ASMFLAGS += -mgeneral-regs-only

# build method
include $(COMMON_DIR)/common.mk

.PRECIOUS: $(BUILD_OBJS) $(DEBUG_OBJS) $(BDIR)/$(BL_TARGET).elf $(DBGDIR)/$(BL_TARGET).elf

.PHONY: clean all debug

all: $(BDIR)/$(BL_TARGET).img

debug: $(DBGDIR)/$(BL_TARGET).img

clean:
	@rm -rf $(BDIR)/* $(DBGDIR)/*