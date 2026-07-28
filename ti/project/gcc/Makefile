SEEKFREE ?= $(abspath ../..)
LIBRARIES := $(SEEKFREE)/libraries
SDK := $(LIBRARIES)/sdk
PROJECT := $(SEEKFREE)/project
BUILD_DIR := build
TARGET := $(BUILD_DIR)/seekfree
APP_C_SOURCE ?=
APP_CPP_SOURCE ?= $(PROJECT)/user/src/main.cpp
PROJECT_CPP_SOURCES ?= $(shell find $(PROJECT)/code -name '*.cpp' -print)
CPP_RUNTIME ?= 1
C_SOURCES := \
	$(APP_C_SOURCE) \
	$(PROJECT)/user/src/isr.c \
	$(wildcard $(LIBRARIES)/zf_common/*.c) \
	$(wildcard $(LIBRARIES)/zf_components/*.c) \
	$(wildcard $(LIBRARIES)/zf_device/*.c) \
	$(wildcard $(LIBRARIES)/zf_driver/*.c) \
	$(wildcard $(SDK)/ti/driverlib/*.c) \
	$(SDK)/ti/driverlib/m0p/sysctl/dl_sysctl_mspm0g1x0x_g3x0x.c \
	$(SDK)/ti_config/ti_msp_dl_config.c \
	$(PROJECT)/gcc/startup.c
CPP_SOURCES := $(APP_CPP_SOURCE) $(PROJECT_CPP_SOURCES)
C_OBJECTS := $(patsubst $(SEEKFREE)/%.c,$(BUILD_DIR)/%.c.o,$(C_SOURCES))
CPP_OBJECTS := $(patsubst $(SEEKFREE)/%.cpp,$(BUILD_DIR)/%.cpp.o,$(CPP_SOURCES))
OBJECTS := $(C_OBJECTS) $(CPP_OBJECTS)
DEPS := $(OBJECTS:.o=.d)

TOOLCHAIN ?=
TOOLCHAIN_PREFIX := $(if $(TOOLCHAIN),$(TOOLCHAIN)/)
CC := $(TOOLCHAIN_PREFIX)arm-none-eabi-gcc
CXX := $(TOOLCHAIN_PREFIX)arm-none-eabi-g++
LINKER ?= $(CXX)
OBJCOPY := $(TOOLCHAIN_PREFIX)arm-none-eabi-objcopy
SIZE := $(TOOLCHAIN_PREFIX)arm-none-eabi-size
PYOCD := uvx --from pyocd pyocd
DEVICE := MSPM0G3507
HOST_CXX ?= c++
HOST_TEST_DIR := $(BUILD_DIR)/host-tests
HOST_TESTS := $(HOST_TEST_DIR)/link_test $(HOST_TEST_DIR)/motor_test \
	$(HOST_TEST_DIR)/program_runner_test $(HOST_TEST_DIR)/menu_test \
	$(HOST_TEST_DIR)/state_machine_demo_test \
	$(HOST_TEST_DIR)/ips114_display_test $(HOST_TEST_DIR)/uart_test \
	$(HOST_TEST_DIR)/pid_test $(HOST_TEST_DIR)/filter_test

CFLAGS := -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -std=gnu11 -Os \
	-ffunction-sections -fdata-sections -fshort-enums -fsigned-char \
	-Wall -MMD -MP \
	-DTI_CPP_RUNTIME=$(CPP_RUNTIME) \
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
CXXFLAGS := $(filter-out -std=gnu11,$(CFLAGS)) -std=gnu++17 \
	-fno-exceptions -fno-rtti -fno-use-cxa-atexit
LDFLAGS := -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -nostartfiles \
	-T linker.ld -Wl,--gc-sections,-Map,$(TARGET).map \
	$(LIBRARIES)/zf_device/zf_device_config.lib \
	--specs=nano.specs --specs=nosys.specs

.PHONY: all clean flash flash-smoke setup smoke test

all: $(TARGET).elf $(TARGET).hex
	$(SIZE) $(TARGET).elf

setup:
	$(PYOCD) pack install $(DEVICE)

flash: all
	$(PYOCD) load -t $(DEVICE) $(TARGET).elf

smoke:
	$(MAKE) BUILD_DIR=build/smoke clean
	$(MAKE) BUILD_DIR=build/smoke \
		APP_C_SOURCE=$(PROJECT)/user/src/hardware_smoke.c \
		APP_CPP_SOURCE= PROJECT_CPP_SOURCES= CPP_RUNTIME=0 LINKER=$(CC) all

flash-smoke:
	$(MAKE) BUILD_DIR=build/smoke clean
	$(MAKE) BUILD_DIR=build/smoke \
		APP_C_SOURCE=$(PROJECT)/user/src/hardware_smoke.c \
		APP_CPP_SOURCE= PROJECT_CPP_SOURCES= CPP_RUNTIME=0 LINKER=$(CC) flash

test: $(HOST_TESTS)
	@for test_binary in $(HOST_TESTS); do $$test_binary; done

$(HOST_TEST_DIR)/link_test: $(SEEKFREE)/test/comm/link_test.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code \
		-I$(SEEKFREE)/test $< -o $@

$(HOST_TEST_DIR)/motor_test: $(SEEKFREE)/test/zdt/motor_test.cpp \
		$(PROJECT)/code/zdt/motor.cpp $(PROJECT)/code/zdt/protocol.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code \
		-I$(SEEKFREE)/test $^ -o $@

$(HOST_TEST_DIR)/program_runner_test: \
		$(SEEKFREE)/test/runtime/program_runner_test.cpp \
		$(PROJECT)/code/runtime/program_runner.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code \
		-I$(SEEKFREE)/test $^ -o $@

$(HOST_TEST_DIR)/menu_test: $(SEEKFREE)/test/ui/menu_test.cpp \
		$(PROJECT)/code/runtime/program_runner.cpp \
		$(PROJECT)/code/ui/menu.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code \
		-I$(SEEKFREE)/test $^ -o $@

$(HOST_TEST_DIR)/state_machine_demo_test: \
		$(SEEKFREE)/test/runtime/state_machine_demo_test.cpp \
		$(PROJECT)/code/program/state_machine_demo.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code \
		-I$(SEEKFREE)/test $^ -o $@

$(HOST_TEST_DIR)/ips114_display_test: \
		$(SEEKFREE)/test/platform/ips114_display_test.cpp \
		$(PROJECT)/code/platform/ips114_display.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror \
		-I$(SEEKFREE)/test/stubs -I$(PROJECT)/code $^ -o $@

$(HOST_TEST_DIR)/uart_test: $(SEEKFREE)/test/platform/uart_test.cpp \
		$(PROJECT)/code/platform/uart.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror \
		-I$(SEEKFREE)/test/stubs -I$(PROJECT)/code $^ -o $@

$(HOST_TEST_DIR)/pid_test: $(SEEKFREE)/test/control/pid_test.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code $< -o $@

$(HOST_TEST_DIR)/filter_test: $(SEEKFREE)/test/control/filter_test.cpp
	@mkdir -p $(dir $@)
	$(HOST_CXX) -std=c++17 -Wall -Wextra -Werror -I$(PROJECT)/code $< -o $@

$(TARGET).elf: $(OBJECTS) linker.ld
	@mkdir -p $(BUILD_DIR)
	$(LINKER) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.c.o: $(SEEKFREE)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: $(SEEKFREE)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)
