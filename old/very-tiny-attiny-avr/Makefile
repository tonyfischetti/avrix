MCU = attiny85
F_CPU = 8000000UL
PROGRAMMER = usbtiny

CC = avr-g++
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=gnu++11

SRC = main.cpp
OBJ = main.elf
HEX = main.hex

all: $(HEX)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

$(HEX): $(OBJ)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(HEX)
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

fuses:
	# Example: 8 MHz internal clock, no clock divide
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) \
	  -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

clean:
	rm -f *.elf *.hex
