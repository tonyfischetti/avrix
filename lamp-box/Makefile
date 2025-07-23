
# .DELETE_ON_ERROR:


MCU        := attiny85
F_CPU      := 8000000UL
TARGET     := main
SRC_DIR    := src
INC_DIR    := include
BUILD_DIR  := build
HAL_LOC    := ../avril

CC         := avr-gcc
CXX        := avr-g++
OBJCOPY    := avr-objcopy
OBJDUMP    := avr-objdump
SIZE       := avr-size
AVRDUDE    := avrdude
PROGRAMMER := usbtiny

SRC        := $(wildcard $(SRC_DIR)/*.cpp)
SRC        += $(wildcard $(HAL_LOC)/src/*.cpp)
SRC        += $(wildcard $(HAL_LOC)/src/**/*.cpp)
SRC        += $(wildcard $(HAL_LOC)/src/**/*.S)

CFLAGS 		:= -mmcu=$(MCU) -DF_CPU=$(F_CPU)

CXXFLAGS   := -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CXXFLAGS   += -I$(INC_DIR) -I$(HAL_LOC)/include
CXXFLAGS   += -I$(HAL_LOC)/include/utils
CXXFLAGS   += -Os -Wall -std=gnu++2a -flto -ffunction-sections
CXXFLAGS   += -fdata-sections -Wextra -Wpedantic -Wshadow
CXXFLAGS   +=  # -Wconversion # -Werror
CXXFLAGS   += -Wsign-conversion -Wunreachable-code -Weffc++
CXXFLAGS   += -Wmissing-braces -Wnon-virtual-dtor -Winline
CXXFLAGS   += -Wnormalized -Wold-style-cast
CXXFLAGS   += -Wreturn-local-addr -Wsuggest-final-types
CXXFLAGS   += -Wsuggest-override -Wvirtual-move-assign
CXXFLAGS   += -Wuninitialized # -Wswitch-enum -Wswitch

LDFLAGS    := -Wl,--gc-sections -flto


all: $(BUILD_DIR)/neopixel-asm.o $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET)-disasm.txt $(BUILD_DIR)/$(TARGET).hex

debug:
	echo $(SRC)

#  TODO  improve (.S)

$(BUILD_DIR)/$(TARGET)-disasm.txt: $(BUILD_DIR)/$(TARGET).elf
	$(OBJDUMP) -d $^ > $@

$(BUILD_DIR)/%.elf: $(SRC) $(BUILD_DIR)/neopixel-asm.o
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@
	$(SIZE) -G --mcu=$(MCU) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(BUILD_DIR)/neopixel-asm.o: src/neopixel-asm.S
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -Os -c $< -o $@

.PHONY: flash
flash: $(BUILD_DIR)/$(TARGET).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

.PHONY: fuses
fuses:
	avrdude -c $(PROGRAMMER) -p $(MCU) -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m # 8 MHz

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: monitor
monitor:
	echo "IMPLEMENT ME"


