
.DELETE_ON_ERROR:

BOARD = ATTinyCore:avr:attinyx5:chip=85,clock=8internal
PROJECT = .
BUILD_DIR = $(PROJECT)/build
PORT = /dev/tty.usbserial-B0004H20
HEX_PATH = $(BUILD_DIR)/ATTinyCore.avr.attinyx5/candle.ino.hex


.PHONY: all
all:
	arduino-cli compile --fqbn $(BOARD) $(PROJECT) -e

.PHONY: upload
upload:
	avrdude -c usbtiny -p t85 -U flash:w:$(HEX_PATH):i

.PHONY: fuses
fuses:
	avrdude -c usbtiny -p t85 -U lfuse:w:0xE2:m -U hfuse:w:0xDF:m -U efuse:w:0xFF:m

.PHONY: monitor
monitor:
	arduino-cli monitor -p $(PORT)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

