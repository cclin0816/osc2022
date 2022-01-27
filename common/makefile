# header
HDRS = $(shell find $(IDIR) -type f)

# depenency
OBJS = $(shell find $(SDIR) -type f | sed -e 's/\.[^.]*$$/.o/')
BUILD_OBJS = $(OBJS:$(SDIR)/%.o=$(BDIR)/%.o)
DEBUG_OBJS = $(OBJS:$(SDIR)/%.o=$(DBGDIR)/%.o)

# build method
include $(COMMON_DIR)/common.mk

.PHONY: clean

all: $(BUILD_OBJS)

debug: $(DEBUG_OBJS)

clean:
	@rm -rf $(BDIR)/* $(DBGDIR)/*
