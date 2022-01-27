# release build

# asm files
$(BDIR)/%.o: $(SDIR)/%.s $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(ASM) $(ASMFLAGS) -c $< -o $@

$(BDIR)/%.o: $(SDIR)/%.S $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(ASM) $(ASMFLAGS) -c $< -o $@

$(BDIR)/%.o: $(SDIR)/%.asm $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(ASM) $(ASMFLAGS) -c $< -o $@

# c files
$(BDIR)/%.o: $(SDIR)/%.c $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(OPTIMIZE) $(LTO) $(INC_DIR) -c $< -o $@

# cpp files
$(BDIR)/%.o: $(SDIR)/%.cpp $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(OPTIMIZE) $(LTO) $(INC_DIR) -c $< -o $@

$(BDIR)/%.o: $(SDIR)/%.cc $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(OPTIMIZE) $(LTO) $(INC_DIR) -c $< -o $@

$(BDIR)/%.o: $(SDIR)/%.cxx $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(OPTIMIZE) $(LTO) $(INC_DIR) -c $< -o $@

# linking
$(BDIR)/%.elf: $(LINKER) $(BUILD_OBJS) $(LIB_BUILD_OBJS) $(MAKEFILE_DEP)
	$(LD) -T $(LINKER) $(LDFLAGS) -O2 --lto-O3 -o $@ $(BUILD_OBJS) $(LIB_BUILD_OBJS)

# debug build

# asm files
$(DBGDIR)/%.o: $(SDIR)/%.s $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(ASM) $(ASMFLAGS) $(DBGFLAGS) -c $< -o $@

$(DBGDIR)/%.o: $(SDIR)/%.S $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(ASM) $(ASMFLAGS) $(DBGFLAGS) -c $< -o $@

$(DBGDIR)/%.o: $(SDIR)/%.asm $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(ASM) $(ASMFLAGS) $(DBGFLAGS) -c $< -o $@

# c files
$(DBGDIR)/%.o: $(SDIR)/%.c $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CC) $(CFLAGS) $(DBGFLAGS) $(INC_DIR) -c $< -o $@

# cpp files
$(DBGDIR)/%.o: $(SDIR)/%.cpp $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(INC_DIR) -c $< -o $@

$(DBGDIR)/%.o: $(SDIR)/%.cc $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(INC_DIR) -c $< -o $@

$(DBGDIR)/%.o: $(SDIR)/%.cxx $(HDRS) $(MAKEFILE_DEP)
	@mkdir -p `dirname $@`
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) $(INC_DIR) -c $< -o $@

$(DBGDIR)/%.elf: $(LINKER) $(DEBUG_OBJS) $(LIB_DEBUG_OBJS) $(MAKEFILE_DEP)
	$(LD) -T $(LINKER) $(LDFLAGS) $(DBGFLAGS) -o $@ $(DEBUG_OBJS) $(LIB_DEBUG_OBJS)


%.img: %.elf $(MAKEFILE_DEP)
	$(OBJCOPY) -O binary $< $@