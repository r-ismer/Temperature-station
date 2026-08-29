CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
SIZE=arm-none-eabi-size

PROJECT_ROOT=.
BUILD_DIR=$(PROJECT_ROOT)/_Build
PROJECT_NAME=weather_station

INCLUDE_PATHS=\
	-I$(PROJECT_ROOT)/app/Include \
	-I$(PROJECT_ROOT)/bsp/Include \
	-I$(PROJECT_ROOT)/graphics_engine/Include \
	-I$(PROJECT_ROOT)/graphics_engine/Private_Include \
	-I$(PROJECT_ROOT)/stm32f103/core/Include \
	-I$(PROJECT_ROOT)/stm32f103/peripherals/Include \
	-I$(PROJECT_ROOT)/stm32f103/peripherals/Include/Legacy \
	-I$(PROJECT_ROOT)/freertos/Include \
	-I$(PROJECT_ROOT)/usb_device/core/Include \
	-I$(PROJECT_ROOT)/usb_device/class/cdc/Include

SOURCES=\
	$(wildcard ./app/Source/*.c) \
	$(wildcard ./bsp/Source/*.c) \
	$(wildcard ./graphics_engine/Source/*.c) \
	$(wildcard ./stm32f103/core/Source/*.c) \
	$(wildcard ./stm32f103/peripherals/Source/*.c) \
	$(wildcard ./freertos/Source/*.c) \
	$(wildcard ./usb_device/core/Source/*.c) \
	$(wildcard ./usb_device/class/cdc/Source/*.c)

OBJECTS_PATH=_Build

CFLAGS=$(INCLUDE_PATHS) -Wall -Os -mcpu=cortex-m3 -mthumb -g3 -ffunction-sections -fdata-sections -DSTM32F103xG -DUSE_HAL_DRIVER -DDEBUG
LDFLAGS=-mcpu=cortex-m0 -mthumb -Wl,--gc-sections -Wl,-Map=$(BUILD_DIR)/$(PROJECT_NAME).map -T$(PROJECT_ROOT)/bsp/Loader/STM32F103RGTX.ld

OBJECTS=$(patsubst %.c,$(OBJECTS_PATH)/%.o,$(SOURCES))

STARTUP_FILE=stm32f103\core\Startup\startup_stm32f103rgtx.s

# When make or make all is called, first create the build folder if it does ot exist, then build all .o files in the Build folder, finally link them to crteate the final .elf and .map files
all: $(BUILD_DIR) $(OBJECTS) $(BUILD_DIR)/$(PROJECT_NAME).elf

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(PROJECT_NAME).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(STARTUP_FILE) -o $@
	$(SIZE) $@

# Compile each .c file to .o file in the Build folder
$(OBJECTS_PATH)/%.o: %.c
	@if not exist "$(dir $@)" mkdir "$(subst /,\,$(dir $@))"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(subst /,\,$(BUILD_DIR))"
	@mkdir "$(subst /,\,$(BUILD_DIR))"
