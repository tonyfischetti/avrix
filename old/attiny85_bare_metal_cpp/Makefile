MCU = attiny85
F_CPU = 8000000UL
TARGET = main
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

CXX = avr-g++
OBJCOPY = avr-objcopy
SIZE = avr-size
AVRDUDE = avrdude
PROGRAMMER = usbtiny

SRC = $(wildcard $(SRC_DIR)/*.cpp)
CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++2a -I$(INC_DIR) -ffunction-sections -fdata-sections
LDFLAGS = -Wl,--gc-sections

all: $(BUILD_DIR)/$(TARGET).hex

$(BUILD_DIR)/%.elf: $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	$(SIZE) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

clean:
	rm -rf $(BUILD_DIR)
