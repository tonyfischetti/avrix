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
CXXFLAGS   := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++17 -I$(INC_DIR) -ffunction-sections -fdata-sections
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
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

.PHONY: fuses
fuses:
	avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0x62:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m
# 1 MHz avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0x62:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m
# 8 MHz avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: monitor
monitor:
	echo "IMPLEMENT ME"

