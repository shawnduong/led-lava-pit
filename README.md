# LED Lava Pit

## Compilation

Install `arduino-cli`, install the ESP32 Arduino core, make sure the required libraries are available, and connect the ESP32 board over USB:

```sh
$ arduino-cli core update-index
$ arduino-cli core install esp32:esp32
```

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
- Board: `esp32:esp32:esp32`
- Serial port: `/dev/ttyUSB0`
- Library directory: `$HOME/doc/appdata/Arduino/libraries/`

Override settings when needed:
```sh
$ make flash PORT=/dev/ttyACM0
$ make flash BOARD=esp32:esp32:esp32s3
```

After flashing firmware, view serial output with:

```sh
$ make monitor
```

The monitor defaults to 9600 baud and can be adjusted with `BAUD`:

```sh
$ make monitor PORT=/dev/ttyUSB0 BAUD=115200
```
