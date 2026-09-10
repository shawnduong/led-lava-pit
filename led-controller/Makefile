ARDUINO_CLI ?= arduino-cli
PORT ?= /dev/ttyACM0
BOARD ?= arduino:avr:uno
BAUD ?= 9600
LIBRARIES ?= $(HOME)/doc/appdata/Arduino/libraries/

.PHONY: compile flash monitor

compile:
	$(ARDUINO_CLI) compile -b "$(BOARD)" --libraries "$(LIBRARIES)"

flash:
	$(ARDUINO_CLI) compile -b "$(BOARD)" --libraries "$(LIBRARIES)" \
	&& $(ARDUINO_CLI) upload -b "$(BOARD)" -p "$(PORT)"

monitor:
	$(ARDUINO_CLI) monitor -p "$(PORT)" -c "$(BAUD)"
