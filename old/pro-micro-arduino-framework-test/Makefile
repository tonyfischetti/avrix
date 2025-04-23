MCU   := atmega32u4
AVRDUDE    := avrdude
PROGRAMMER := avr109
BOARD_FQBN = arduino:avr:leonardo
PORT  :=/dev/cu.usbmodem1401
SRC_DIR = src
INCLUDE_DIR = include
HEX = build/pro-micro-arduino-framework-test.ino.hex

all: build

.PHONY: build
build:
	arduino-cli compile --fqbn $(BOARD_FQBN) --build-path ./build .

.PHONY: flash
flash:
	$(AVRDUDE) -c $(PROGRAMMER) -v -p $(MCU) -P$(PORT) -D -Uflash:w:$(HEX):i

clean:
	rm -rf ./build

