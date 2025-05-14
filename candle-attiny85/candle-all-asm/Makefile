.DELETE_ON_ERROR:

MCU        := attiny85
F_CPU      := 1000000UL
PROGRAMMER := usbtiny

MAIN       := main

CFLAGS := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -O0

# don't link to libc, etc...
CFLAGS += -nostdlib
# No C runtime*
CFLAGS += -nostartfiles
# tells `ld` to place .text (code) in 0x0000 (reset address)
CFLAGS += -Wl,-Ttext=0
# -Wl passes contents to the linker

all: $(MAIN).hex

$(MAIN).hex: $(MAIN).elf
	avr-objcopy -O ihex -R .eeprom $< $@

$(MAIN).elf: $(MAIN).S
	avr-gcc $(CFLAGS) -o $@ $<


.PHONY: flash
flash: $(MAIN).hex
	avrdude -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<:i

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(MAIN).elf
	rm -f $(MAIN).hex


