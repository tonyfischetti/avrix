.DELETE_ON_ERROR:

MCU        := attiny85
F_CPU      := 1000000UL
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

CXXFLAGS   := -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CXXFLAGS   += -I$(INC_DIR)
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

$(SRC): include/attiny85-hal.hpp

#  TODO  have it depend on HAL HPP, too!!!
$(BUILD_DIR)/%.elf: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(BUILD_DIR)/main.elf: include/attiny85-hal.hpp

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

.PHONY: fuses
fuses:
	avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0x62:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m # 1 MHz
# avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m # 8 MHz

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

