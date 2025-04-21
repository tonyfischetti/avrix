MCU = attiny85
F_CPU = 8000000UL
PROGRAMMER = usbtiny


CORE_BASE = $(HOME)/Library/Arduino15/packages/ATTinyCore/hardware/avr/1.5.2
CORE_DIR = $(CORE_BASE)/cores/tiny
VARIANT_DIR = $(CORE_BASE)/variants/tinyX8

CC = avr-g++
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++11
INCLUDES = -I$(CORE_DIR) -I$(VARIANT_DIR) -Iinclude
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

PROJECT_NAME = firmware

SRC_DIR = src
BUILD_DIR = build
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

# CORE_OBJS = \
#   $(patsubst %.c, $(BUILD_DIR)/%.o, $(shell find $(CORE_DIR) -name "*.c")) \
#   $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(shell find $(CORE_DIR) -name "*.cpp")) \
#   $(patsubst %.S, $(BUILD_DIR)/%.o, $(shell find $(CORE_DIR) -name "*.S"))
CORE_OBJS = \
  $(patsubst $(CORE_DIR)/%.c, $(BUILD_DIR)/core/%.o, $(wildcard $(CORE_DIR)/*.c)) \
  $(patsubst $(CORE_DIR)/%.cpp, $(BUILD_DIR)/core/%.o, $(wildcard $(CORE_DIR)/*.cpp)) \
  $(patsubst $(CORE_DIR)/%.S, $(BUILD_DIR)/core/%.o, $(wildcard $(CORE_DIR)/*.S))

ELF = $(BUILD_DIR)/$(PROJECT_NAME).elf
HEX = $(BUILD_DIR)/$(PROJECT_NAME).hex

# === RULES ===

all: $(HEX)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS) $(INCLUDES) -c $< -o $@

# $(BUILD_DIR)/$(CORE_DIR)/wiring_pulse.o: $(CORE_DIR)/wiring_pulse.S
# 	@mkdir -p $(dir $@)
# 	avr-gcc $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@
$(BUILD_DIR)/core/%.o: $(CORE_DIR)/%.S
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

$(ELF): $(OBJ_FILES) $(CORE_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(HEX)
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -U flash:w:$<:i

fuses:
	# Internal 8MHz, no clock division
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) \
	  -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

clean:
	rm -rf $(BUILD_DIR)

print-core-objs:
	echo $(CORE_OBJS)
