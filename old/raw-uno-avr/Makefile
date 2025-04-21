MCU=atmega328p
F_CPU=16000000UL
BAUD=115200
PROGRAMMER=arduino
PORT=/dev/tty.usbmodem1401

CC=avr-gcc
OBJCOPY=avr-objcopy
AVRDUDE=avrdude

CFLAGS=-mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os
LDFLAGS=-mmcu=$(MCU)

TARGET=blink
SRC=$(TARGET).c

all: $(TARGET).hex

%.elf: $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(TARGET).hex
	$(AVRDUDE) -v -patmega328p -carduino -P$(PORT) -b$(BAUD) -D -Uflash:w:$(TARGET).hex:i

clean:
	rm -f *.elf *.hex
