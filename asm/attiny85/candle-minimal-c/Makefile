.DELETE_ON_ERROR:

MCU        := attiny85
F_CPU      := 1000000UL
PROGRAMMER := usbtiny

CFLAGS := -mmcu=$(MCU) -DF_CPU=$(F_CPU)

#  TODO  improve this makefile

all: main.hex

main.hex: main.elf
	avr-objcopy -O ihex -R .eeprom main.elf main.hex

main.elf: main.o lfsr.o
	avr-gcc $(CFLAGS) main.o lfsr.o -o $@

lfsr.o: lfsr.S
	avr-gcc $(CFLAGS) -Os -c $< -o $@

main.o: main.c
	avr-gcc $(CFLAGS) -Os -c $< -o $@

.PHONY: flash
flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

.PHONY: clean
clean:
	rm -f *.o
	rm -f main.elf
	rm -f main.hex
