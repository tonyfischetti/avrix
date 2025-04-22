MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200
PORT=/dev/cu.usbmodem11401

ARDUINO_DIR = $(HOME)/.arduino15/packages/arduino/hardware/avr/1.8.6
CORE_DIR = $(ARDUINO_DIR)/cores/arduino
VARIANT_DIR = $(ARDUINO_DIR)/variants/standard

# === TOOLS ===
CC = avr-g++
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++17
INCLUDES = -I$(CORE_DIR) -I$(VARIANT_DIR) -Iinclude
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

SRC_DIR = src
BUILD_DIR = build
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_FILES))

CORE_OBJS = \
  $(patsubst %.c, $(BUILD_DIR)/%.o, $(shell find $(CORE_DIR) -name "*.c")) \
  $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(shell find $(CORE_DIR) -name "*.cpp")) \
  $(patsubst %.S, $(BUILD_DIR)/%.o, $(shell find $(CORE_DIR) -name "*.S"))

ELF = $(BUILD_DIR)/firmware.elf
HEX = $(BUILD_DIR)/firmware.hex

# === RULES ===

all: $(HEX)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile Arduino core sources
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	avr-g++ $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/$(CORE_DIR)/wiring_pulse.o: $(CORE_DIR)/wiring_pulse.S
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

# Link everything
$(ELF): $(OBJ_FILES) $(CORE_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Convert to HEX
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Flash
flash: $(HEX)
	$(AVRDUDE) -v -patmega328p -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$<:i

# Clean
clean:
	rm -rf $(BUILD_DIR)

print-core-sources:
	@find $(CORE_DIR) -name "*.S"

print-core-objs:
	echo $(CORE_OBJS)

gen-compile-commands:
	bear -- make


#  TODO  phonies
.PHONY: monitor
monitor:
	arduino-cli monitor -p $(PORT)

