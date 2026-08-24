# PlatformIO Project

```bash
$ pio --version
PlatformIO Core, version 6.1.19
```

## Hardware

* ESP32-S3-WROOM-1U-N16-R8

### Schematics

```text
gpio02 -- [ 220 Ohm ] -- [A(+) LED2 K(-)] -- GND
gpio47 -- [ _/  BTN ] -- GND
```

## Commands

```shell
# Build project
$ pio run

# Upload firmware
$ pio run -t upload

# Monoitor
$ pio run -t monitor

# Clean build files
$ pio run -t clean
```
