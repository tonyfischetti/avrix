MCU         = attiny85
F_CPU       = 8000000UL
PROGRAMMER  = usbtiny
PROJECT     = firmware

CORE_BASE   = $(HOME)/Library/Arduino15/packages/ATTinyCore/hardware/avr/1.5.2
CORE_DIR    = $(CORE_BASE)/cores/tiny
VARIANT_DIR = $(CORE_BASE)/variants/tinyX8

SRC_DIR     = src
INCLUDE_DIR = include
BUILD_DIR   = build

CFLAGS_C   = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
CFLAGS_CPP = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++11
INCLUDES   = -I$(CORE_DIR) -I$(VARIANT_DIR) -I$(INCLUDE_DIR)

SRC_C_FILES     := $(wildcard $(SRC_DIR)/*.c)
SRC_CPP_FILES   := $(wildcard $(SRC_DIR)/*.cpp)


CORE_C_FILES := $(filter-out $(CORE_DIR)/WInterrupts.c, $(filter-out $(CORE_DIR)/wiring_analog.c, $(wildcard $(CORE_DIR)/*.c)))

CORE_CPP_FILES  := $(wildcard $(CORE_DIR)/*.cpp)
CORE_S_FILES    := $(wildcard $(CORE_DIR)/*.S)

$(info CORE_C_FILES = $(CORE_C_FILES))

OBJ_SRC_C     := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_C_FILES))
OBJ_SRC_CPP   := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC_CPP_FILES))
OBJ_CORE_C    := $(patsubst $(CORE_DIR)/%.c, $(BUILD_DIR)/core/%.o, $(CORE_C_FILES))
OBJ_CORE_CPP  := $(patsubst $(CORE_DIR)/%.cpp, $(BUILD_DIR)/core/%.o, $(CORE_CPP_FILES))
OBJ_CORE_S    := $(patsubst $(CORE_DIR)/%.S, $(BUILD_DIR)/core/%.o, $(CORE_S_FILES))

ALL_OBJS = $(OBJ_SRC_C) $(OBJ_SRC_CPP) $(OBJ_CORE_C) $(OBJ_CORE_CPP) $(OBJ_CORE_S)

ELF = $(BUILD_DIR)/$(PROJECT).elf
HEX = $(BUILD_DIR)/$(PROJECT).hex

all: $(HEX)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS_C) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	avr-g++ $(CFLAGS_CPP) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/core/%.o: $(CORE_DIR)/%.c
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS_C) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/core/%.o: $(CORE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	avr-g++ $(CFLAGS_CPP) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/core/%.o: $(CORE_DIR)/%.S
	@mkdir -p $(dir $@)
	avr-gcc $(CFLAGS_C) $(INCLUDES) -x assembler-with-cpp -c $< -o $@

$(ELF): $(ALL_OBJS)
	avr-g++ $(CFLAGS_CPP) $(INCLUDES) $^ -o $@

$(HEX): $(ELF)
	avr-objcopy -O ihex -R .eeprom $< $@

upload: $(HEX)
	avrdude -v -p $(MCU) -c $(PROGRAMMER) -U flash:w:$<:i

fuses:
	avrdude -c $(PROGRAMMER) -p $(MCU) \
	  -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

clean:
	rm -rf $(BUILD_DIR)
