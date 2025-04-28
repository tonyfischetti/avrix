.DELETE_ON_ERROR:

MCU        := atmega328p
MCUAVRD    := m328p
F_CPU      := 16000000UL
BAUD       := 9600
TARGET     := main
SRC_DIR    := src
INC_DIR    := include
BUILD_DIR  := build

CXX        := avr-g++
OBJCOPY    := avr-objcopy
SIZE       := avr-size
AVRDUDE    := avrdude
PROGRAMMER := usbtiny

SRC        := $(wildcard $(SRC_DIR)/*.cpp)
CXXFLAGS   := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -I$(INC_DIR)
CXXFLAGS   += -Os -Wall -std=gnu++17 -ffunction-sections -fdata-sections
CXXFLAGS   += -Wextra -Wpedantic -Wshadow -Wconversion # -Werror
CXXFLAGS   += -Wsign-conversion -Wunreachable-code -Weffc++
CXXFLAGS   += -Wmissing-braces -Wnon-virtual-dtor -Winline
CXXFLAGS   += -Wnormalized -Wold-style-cast
CXXFLAGS   += -Wreturn-local-addr -Wsuggest-final-types
CXXFLAGS   += -Wsuggest-override -Wvirtual-move-assign
CXXFLAGS   += -Wuninitialized -Wswitch-enum -Wswitch

LDFLAGS    := -Wl,--gc-sections

all: $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/%.elf: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCUAVRD) -U flash:w:$<:i

.PHONY: fuses
fuses:
	avrdude -c $(PROGRAMMER) -p $(MCUAVRD) -U lfuse:w:0xD6:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m  # external crystal
# avrdude -c $(PROGRAMMER) -p $(MCUAVRD) -U lfuse:w:0x62:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m  # 1MHz
# avrdude -c $(PROGRAMMER) -p $(MCUAVRD) -U lfuse:w:0xE2:m -U hfuse:w:0xD9:m -U efuse:w:0xFF:m  # 8MHz

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: monitor
monitor:
	echo "IMPLEMENT ME"

