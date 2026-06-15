MCU := atmega328p
F_CPU := 16000000UL

CC := avr-gcc
OBJCOPY := avr-objcopy
SIZE := avr-size
AVRDUDE := avrdude

# FIX 1: Safely extract the current directory name without literal ()
TARGET := $(notdir $(patsubst %/,%,$(CURDIR)))
BUILD_DIR := build

DRIVERS_DIR := ../../../drivers
LIB_DIR := $(DRIVERS_DIR)/build
PROGRAMMER := arduino
BAUD := 115200
PORT := /dev/ttyUSB0

CFLAGS := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Wall -Os
INCLUDES := -I. -I$(DRIVERS_DIR)

all: $(BUILD_DIR)/$(TARGET).hex

# Triggers compilation inside the drivers workspace
$(LIB_DIR)/libdrv.a:
	$(MAKE) -C $(DRIVERS_DIR)

# Dynamically create the build folder when needed
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(TARGET).elf: $(TARGET).c $(LIB_DIR)/libdrv.a | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -L$(LIB_DIR) -ldrv -o $@

$(BUILD_DIR)/$(TARGET).hex: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) $<

flash: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p m328p -P $(PORT) -b $(BAUD) -U flash:w:$<:i

clean:
	rm -rf $(BUILD_DIR)

clean-drivers:
	$(MAKE) -C $(DRIVERS_DIR) clean

clean-all: clean clean-drivers

.PHONY: all clean flash clean-drivers clean-all