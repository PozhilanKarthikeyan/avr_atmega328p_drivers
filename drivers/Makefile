MCU := atmega328p

CC := avr-gcc
AR := avr-ar
OBJDUMP := avr-objdump
NM := avr-nm
SIZE := avr-size

CFLAGS := -mmcu=$(MCU) -Wall -Wextra -Os -g

BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj

DRIVER_SRCS := $(wildcard */*.c)

DRIVER_OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(DRIVER_SRCS))

LIB := $(BUILD_DIR)/libdrv.a

all: $(LIB)

$(LIB): $(DRIVER_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(AR) rcs $@ $^
	@$(NM) --print-size --size-sort -r $(LIB)
	@$(SIZE) --format=avr -d $(LIB) --mcu=$(MCU) 

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I. -Icommon -c $< -o $@

disasm: $(LIB)
	$(OBJDUMP) -t -d $(LIB) > $(BUILD_DIR)/libdrv.asm

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean disasm