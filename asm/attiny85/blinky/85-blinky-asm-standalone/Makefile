.DELETE_ON_ERROR:

MCU        := attiny85
F_CPU      := 1000000UL
PROGRAMMER := usbtiny

CFLAGS := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O0

# don't link to libc, etc... (adds ~2KB unnecessarily)
CFLAGS += -nostdlib
# No C runtime*
CFLAGS += -nostartfiles

all: blink.hex

blink.hex: blink.elf
	avr-objcopy -O ihex -R .eeprom $< $@

blink.elf: blink.s
	avr-gcc $(CFLAGS) -o $@ $<


.PHONY: flash
flash: blink.hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

.PHONY: clean
clean:
	rm -f *.o
	rm -f blink.elf
	rm -f blink.hex


# * the C runtime would
#     - set up the stack
#     - initialize static or global variables
#     - call global C++ constructors
#     - calls `main()`
