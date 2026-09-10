# LED Lava Pit

## Compilation

Install `arduino-cli`, make sure the Arduino AVR core and required libraries are available, and connect the board over USB.

From the repository root:
```sh
$ cd led-controller/
$ make compile
```

To compile and upload the firmware, use:
```sh
$ make flash
```

The default settings are:
- Board: `arduino:avr:uno`
- Serial port: `/dev/ttyACM0`
- Library directory: `$HOME/doc/appdata/Arduino/libraries/`

Override settings when needed:
```sh
$ make flash PORT=/dev/ttyUSB0
$ make flash BOARD=arduino:avr:nano
```

After flashing firmware, view serial output with:

```sh
$ make monitor
```

The monitor defaults to 9600 baud and can be adjusted with `BAUD`:

```sh
$ make monitor PORT=/dev/ttyUSB0 BAUD=115200
```
