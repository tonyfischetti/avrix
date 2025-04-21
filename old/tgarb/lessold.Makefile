# === MCU & Arduino Core Settings ===
MCU         = attiny85
F_CPU       = 8000000UL
PROGRAMMER  = usbtiny
BAUD        = 19200  # unused by usbtiny

# === Paths ===
CORE_BASE   = $(HOME)/Library/Arduino15/packages/ATTinyCore/hardware/avr/1.5.2
CORE_DIR    = $(CORE_BASE)/cores/tiny
VARIANT_DIR = $(CORE_BASE)/variants/tinyX8

SRC_DIR     = src
INCLUDE_DIR = include
BUILD_DIR   = build
PROJECT     = firmware

# === Source Files ===
SRC_CPP     := $(wildcard $(SRC_DIR)/*.cpp)
SRC_C       := $(wildcard $(SRC_DIR)/*.c)
OBJ_SRC     := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_CPP)) \
               $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_C))

CORE_C_SRCS   := $(wildcard $(CORE_DIR)/*.c)
CORE_CPP_SRCS := $(wildcard $(CORE_DIR)/*.cpp)
CORE_S_SRCS   := $(wildcard $(CORE_DIR)/*.S)

CORE_OBJS = \
  $(patsubst $(CORE_DIR)/%.c, $(BUILD_DIR)/core/%.o, $(wildcard $(CORE_DIR)/*.c)) \
  $(patsubst $(CORE_DIR)/%.cpp, $(BUILD_DIR)/core/%.o, $(wildcard $(CORE_DIR)/*.cpp)) \
  $(patsubst $(CORE_DIR)/%.S, $(BUILD_DIR)/core/%.o, $(wildcard $(CORE_DIR)/*.S))

# === Tools ===
CC       = avr-g++
CPPFLAGS   = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++11
CFLAGS   = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
INCLUDES = -I$(CORE_DIR) -I$(VARIANT_DIR) -I$(INCLUDE_DIR)
OBJCOPY  = avr-objcopy
AVRDUDE  = avrdude

# === Outputs ===
ELF = $(BUILD_DIR)/$(PROJECT).elf
HEX = $(BUILD_DIR)/$(PROJECT).hex

# === Targets ===

all: $(HEX)

# Build source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/core/%.o: $(CORE_DIR)/%.S
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

# Link
$(ELF): $(OBJ_SRC) $(CORE_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

# Convert to HEX
$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Upload via USBtiny
upload: $(HEX)
	$(AVRDUDE) -v -p $(MCU) -c $(PROGRAMMER) -U flash:w:$<:i

# Optional: Set fuses for 8MHz internal clock
fuses:
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) \
	  -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

clean:
	rm -rf $(BUILD_DIR)

debug:
	echo $(CFLAGS)

print-core-objs:
	echo $(CORE_OBJS)
