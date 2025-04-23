.DELETE_ON_ERROR:

UNAME := $(shell uname)
MCU   := atmega328p
F_CPU := 16000000UL
BAUD  := 115200
PORT  :=/dev/cu.usbmodem1401

ifeq ($(UNAME), Darwin)
	ARDUINO_DIR = $(HOME)/Library/Arduino15/packages/arduino/hardware/avr/1.8.6
else
	ARDUINO_DIR = $(HOME)/.arduino15/packages/arduino/hardware/avr/1.8.6
endif

CORE_DIR    := $(ARDUINO_DIR)/cores/arduino
VARIANT_DIR := $(ARDUINO_DIR)/variants/standard

CC  	   := avr-gcc
CXX        := avr-g++
OBJCOPY    := avr-objcopy
SIZE       := avr-size
AVRDUDE    := avrdude
PROGRAMMER := arduino

CFLAGS 	   := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++17
CXXFLAGS   := $(CFLAGS)
CXXFLAGS   += -ffunction-sections -fdata-sections
CXXFLAGS   += -Wextra -Wpedantic -Wshadow -Wconversion # -Werror
CXXFLAGS   += -Wsign-conversion -Wunreachable-code -Weffc++
CXXFLAGS   += -Wmissing-braces -Wnon-virtual-dtor -Winline
CXXFLAGS   += -Wnormalized -Wold-style-cast
CXXFLAGS   += -Wreturn-local-addr -Wsuggest-final-types
CXXFLAGS   += -Wsuggest-override -Wvirtual-move-assign
CXXFLAGS   += -Wuninitialized -Wswitch-enum -Wswitch

INCLUDES = -I$(CORE_DIR) -I$(VARIANT_DIR) -Iinclude

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
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# arduino core sources
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/$(CORE_DIR)/wiring_pulse.o: $(CORE_DIR)/wiring_pulse.S
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

# link everything
$(ELF): $(OBJ_FILES) $(CORE_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@
	$(SIZE) $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.PHONY: flash
flash: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -v -p $(MCU) -P$(PORT) -b$(BAUD) -D -Uflash:w:$<:i

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: monitor
monitor:
	arduino-cli monitor -p $(PORT)

