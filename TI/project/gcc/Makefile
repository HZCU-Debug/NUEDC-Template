SEEKFREE ?= $(abspath ../..)
LIBRARIES := $(SEEKFREE)/libraries
SDK := $(LIBRARIES)/sdk
PROJECT := $(SEEKFREE)/project
BUILD_DIR := build
TARGET := $(BUILD_DIR)/seekfree
SOURCES := \
	$(PROJECT)/user/src/main.c \
	$(PROJECT)/user/src/isr.c \
	$(wildcard $(LIBRARIES)/zf_common/*.c) \
	$(wildcard $(LIBRARIES)/zf_components/*.c) \
	$(wildcard $(LIBRARIES)/zf_device/*.c) \
	$(wildcard $(LIBRARIES)/zf_driver/*.c) \
	$(wildcard $(SDK)/ti/driverlib/*.c) \
	$(SDK)/ti_config/ti_msp_dl_config.c \
	startup.c

TOOLCHAIN ?= /Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin
CC := $(TOOLCHAIN)/arm-none-eabi-gcc
OBJCOPY := $(TOOLCHAIN)/arm-none-eabi-objcopy
SIZE := $(TOOLCHAIN)/arm-none-eabi-size
PYOCD := uvx --from pyocd pyocd
DEVICE := MSPM0G3507

CFLAGS := -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -std=gnu11 -Os \
	-ffunction-sections -fdata-sections -fshort-enums -fsigned-char -Wall \
	-include gcc_compat.h \
	-D__MSPM0G3507__ \
	-I$(SDK)/third_party/CMSIS/Core/Include \
	-I$(SDK) \
	-I$(SDK)/ti_config \
	-I$(LIBRARIES)/zf_common \
	-I$(LIBRARIES)/zf_components \
	-I$(LIBRARIES)/zf_device \
	-I$(LIBRARIES)/zf_driver \
	-I$(PROJECT)/user/inc \
	-I$(PROJECT)/code
LDFLAGS := -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -nostartfiles \
	-T linker.ld -Wl,--gc-sections,-Map,$(TARGET).map \
	$(LIBRARIES)/zf_device/zf_device_config.lib \
	--specs=nano.specs --specs=nosys.specs

.PHONY: all clean setup flash

all: $(TARGET).elf $(TARGET).hex
	$(SIZE) $(TARGET).elf

setup:
	$(PYOCD) pack install $(DEVICE)

flash: all
	$(PYOCD) load -t $(DEVICE) $(TARGET).elf

$(TARGET).elf: $(SOURCES) linker.ld
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

clean:
	$(RM) -r $(BUILD_DIR)
