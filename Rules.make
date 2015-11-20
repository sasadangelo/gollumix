FUNNYDIR := $(FUNNYDIR)/`basename $(CURDIR)`

.PHONY: do_it_all all sub_dirs

do_it_all: sub_dirs all

# rule to assembly .s files
%.o: %.s
	@echo "[AS] $@"
	@$(AS) -o $@ $<

# rule to compile .c file
.c.o:
	@echo "[CC] $@"
	@$(CC) $(CFLAGS) $< -o  $@

#rule to make sub directories
subdir-list = $(sort $(patsubst %,_subdir_%,$(SUB_DIRS)))
sub_dirs: $(subdir-list)

ifdef SUB_DIRS
$(subdir-list):
	@$(MAKE) -C $(patsubst _subdir_%,%,$@)
endif

# rule to make clean
clean: local_clean
	@echo "CLEAN $(FUNNYDIR)"
	@for i in $(SUB_DIRS); do $(MAKE) clean -C $$i; done

