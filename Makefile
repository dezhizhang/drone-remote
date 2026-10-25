# Native GNU Make build; compatible with macOS /usr/bin/make 3.81.
.DEFAULT_GOAL := all
.DELETE_ON_ERROR:

BUILD ?= debug
ifeq ($(filter $(BUILD),debug release),)
$(error BUILD must be debug or release)
endif

BUILD_DIR := build/make/$(BUILD)
TARGET := $(BUILD_DIR)/drone-remote
ELF := $(TARGET).elf
HEX := $(TARGET).hex
BIN := $(TARGET).bin
LINKER_SCRIPT := linker/STM32F103C8TX_FLASH.ld

# Optionally point TOOLCHAIN_BIN at an Arm GNU Toolchain's bin directory.
TOOLCHAIN_BIN ?=
CROSS_COMPILE ?= arm-none-eabi-
TOOL_PREFIX := $(if $(TOOLCHAIN_BIN),$(TOOLCHAIN_BIN)/,)$(CROSS_COMPILE)
CC := $(TOOL_PREFIX)gcc
OBJCOPY := $(TOOL_PREFIX)objcopy
SIZE := $(TOOL_PREFIX)size
NM := $(TOOL_PREFIX)nm
PYTHON ?= python3
OPENOCD ?= openocd
OPENOCD_CONFIG ?= Config/openocd.cfg

HAL_DIR := Drivers/STM32F1xx_HAL_Driver
CMSIS_DIR := Drivers/CMSIS
DEVICE_DIR := $(CMSIS_DIR)/Device/ST/STM32F1xx
RTOS_DIR := Middlewares/Third_Party/FreeRTOS/Source

APP_SOURCES := $(sort $(wildcard Core/Src/*.c App/Src/*.c BSP/Src/*.c))
HAL_MODULES := hal hal_cortex hal_rcc hal_rcc_ex hal_gpio hal_gpio_ex \
               hal_dma hal_tim hal_tim_ex hal_flash hal_flash_ex hal_pwr hal_uart
HAL_SOURCES := $(addprefix $(HAL_DIR)/Src/stm32f1xx_,$(addsuffix .c,$(HAL_MODULES)))
RTOS_SOURCES := $(addprefix $(RTOS_DIR)/,tasks.c queue.c list.c portable/GCC/ARM_CM3/port.c portable/MemMang/heap_4.c)
C_SOURCES := $(APP_SOURCES) $(HAL_SOURCES) $(RTOS_SOURCES) \
             $(DEVICE_DIR)/Source/Templates/system_stm32f1xx.c
ASM_SOURCES := $(DEVICE_DIR)/Source/Templates/gcc/startup_stm32f103xb.s
OBJECTS := $(addprefix $(BUILD_DIR)/,$(C_SOURCES:.c=.o) $(ASM_SOURCES:.s=.o))
APP_OBJECTS := $(addprefix $(BUILD_DIR)/,$(APP_SOURCES:.c=.o))

INCLUDE_DIRS := Core/Inc Config BSP/Inc App/Inc $(HAL_DIR)/Inc $(HAL_DIR)/Inc/Legacy \
                $(CMSIS_DIR)/Include $(DEVICE_DIR)/Include \
                $(RTOS_DIR)/include $(RTOS_DIR)/portable/GCC/ARM_CM3
CPPFLAGS += -DUSE_HAL_DRIVER -DSTM32F103xB -DUSER_VECT_TAB_ADDRESS \
            $(addprefix -I,$(INCLUDE_DIRS))
CPU_FLAGS := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft
CONFIG_FLAGS_debug := -Og -g3
CONFIG_FLAGS_release := -Os
CFLAGS += $(CPU_FLAGS) -std=c11 $(CONFIG_FLAGS_$(BUILD)) \
          -ffunction-sections -fdata-sections -fno-common -fstack-usage
ASFLAGS += $(CPU_FLAGS) $(CONFIG_FLAGS_$(BUILD)) -x assembler-with-cpp
LDFLAGS += $(CPU_FLAGS) -T$(LINKER_SCRIPT) -nostartfiles \
           -Wl,--gc-sections,--print-memory-usage,-Map=$(TARGET).map

$(APP_OBJECTS): CFLAGS += -Wall -Wextra -Werror -Wshadow -Wundef -Wconversion

.PHONY: all firmware compdb flash check clean size doctor deps help

all: firmware compdb

firmware: $(ELF) $(HEX) $(BIN)

# Derive editor commands from Make's own recipes, without running a compiler.
compdb:
	"$(PYTHON)" scripts/generate_compile_commands.py --make "$(MAKE)" \
	    --build "$(BUILD)" --compiler "$(CC)"

$(ELF): $(OBJECTS) $(LINKER_SCRIPT) Makefile
	@mkdir -p "$(@D)"
	"$(CC)" $(OBJECTS) $(LDFLAGS) -o "$@"
	"$(SIZE)" "$@"

$(HEX): $(ELF)
	"$(OBJCOPY)" -O ihex "$<" "$@"

$(BIN): $(ELF)
	"$(OBJCOPY)" -O binary "$<" "$@"

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p "$(@D)"
	"$(CC)" $(CPPFLAGS) $(CFLAGS) -MMD -MP -c "$<" -o "$@"

$(BUILD_DIR)/%.o: %.s Makefile
	@mkdir -p "$(@D)"
	"$(CC)" $(CPPFLAGS) $(ASFLAGS) -MMD -MP -c "$<" -o "$@"

# Build first, then program + verify + reset. No erase-only target is provided.
flash: check
	@command -v "$(OPENOCD)" >/dev/null 2>&1 || { \
	    printf '%s\n' 'OpenOCD not found. Install it or set OPENOCD=/path/to/openocd.'; exit 1; }
	"$(OPENOCD)" -f "$(OPENOCD_CONFIG)" -c 'program {$(abspath $(ELF))} verify reset exit'

check: all
	"$(PYTHON)" scripts/verify_firmware.py "$(ELF)" --nm "$(NM)" --objcopy "$(OBJCOPY)"

size: $(ELF)
	"$(SIZE)" "$<"

clean:
	$(RM) -r "$(BUILD_DIR)"

doctor:
	@set -e; for tool in "$(CC)" "$(OBJCOPY)" "$(SIZE)" "$(NM)" "$(PYTHON)" "$(OPENOCD)"; do \
	    command -v "$$tool" >/dev/null 2>&1 || { printf 'Missing: %s\n' "$$tool"; exit 1; }; \
	    printf 'Found: %s\n' "$$tool"; \
	done

deps:
	"$(PYTHON)" scripts/fetch_dependencies.py

help:
	@printf '%s\n' \
	  'make [-j4]                Build Debug ELF/HEX/BIN/MAP with ARM GCC' \
	  'make BUILD=release [-j4]  Build Release in a separate directory' \
	  'make flash               Build, program via ST-Link/SWD, verify and reset' \
	  'make check               Check vectors, HAL/RTOS hooks and memory bounds' \
	  'make compdb              Refresh compile_commands.json for editor diagnostics' \
	  'make clean               Remove only the selected Make build directory' \
	  'make doctor              Check build/flash tools on PATH' \
	  'make deps                Restore pinned upstream sources (network needed)' \
	  'Optional: TOOLCHAIN_BIN=/path/to/bin OPENOCD=/path/to/openocd' \
	  'Confirmed MCU: STM32F103C8T6 (64 KiB Flash / 20 KiB RAM)'

-include $(OBJECTS:.o=.d)
