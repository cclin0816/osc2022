IDIR = include
SDIR = src
BDIR = build
DBGDIR = debug

HDRS = $(shell find $(IDIR) -type f)
SRCS = $(shell find $(SDIR) -type f)
OBJS = $(shell find $(SDIR) -type f | sed -e 's/\.[^.]*$$/.o/')
BUILD_OBJS = $(OBJS:$(SDIR)/%.o=$(BDIR)/%.o)
DEBUG_OBJS = $(OBJS:$(SDIR)/%.o=$(DBGDIR)/%.o)

.PHONY: clean

$(BDIR)/%.o: $(SDIR)/%.c $(HDRS)
	@mkdir -p `dirname $@`
	@touch $@

$(BDIR)/%.o: $(SDIR)/%.s $(HDRS)
	@mkdir -p `dirname $@`
	@touch $@

$(BDIR)/%.o: $(SDIR)/%.cpp $(HDRS)
	@mkdir -p `dirname $@`
	@touch $@

$(DBGDIR)/%.o: $(SDIR)/%.c $(HDRS)
	@mkdir -p `dirname $@`
	@touch $@

$(DBGDIR)/%.o: $(SDIR)/%.s $(HDRS)
	@mkdir -p `dirname $@`
	@touch $@

$(DBGDIR)/%.o: $(SDIR)/%.cpp $(HDRS)
	@mkdir -p `dirname $@`
	@touch $@

all: $(BUILD_OBJS)

debug: $(DEBUG_OBJS)

clean:
	@rm -rf $(BDIR)/* $(DBGDIR)/*
