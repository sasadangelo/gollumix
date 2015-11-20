#
# tools/Makefile
#

include $(RULES)

BUILD_PRG=build

all: $(BUILD_PRG)

$(BUILD_PRG):
	@echo "[CC] $@"
	@$(CC) $(CPPFLAGS) -o build build.c 

local_clean:
	@rm -f $(BUILD_PRG)
