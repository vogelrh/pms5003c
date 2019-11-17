# pms5003c

A Linux C library for communicating with the Plantower PMS5003 particulate sensor. This library has only been tested on Raspian (Debian) Linux but it should be compatible with other Linux systems since it uses POSIX compliant serial IO libraries.

This library took its inspiration from and is modeled after the [Pimoroni Python library](https://github.com/pimoroni/pms5003-python).

However, unlike the Pimoroni library, pms5003c does not make use of the PMS5003's `SET_CONTROL` and `RESET_CONTROL` pins.

## Useage
Simply add the `pms5003.c` and the `pms5003.h` files to your C project.

1. call `pms_init()` to initialize the UART
2. declare a `PMS5003_DATA` structure and pass it to `read_pms5003_data`
3. process the output

See `example.c` for more detail.

## Porting
Since this code uses fairly standard Linux libraries, it should not be difficult to get this library up and running on different Linux systems.

There is one issue you must be aware of. The PMS5003 returns data as two  uint8_t bytes in Big Endian format. Therefore it is important to know the Endedness of the system running the library. There is a `#define LITTLE_ENDED` statement in pms5003.h that can be changed from the current value of 1 to 0 for Big Ended systems.

## Raspberry Pi UART Setup
The UART on the Raspberry Pi needs to be reconfigured for use with the GPIO pins 8 and 10. The steps are straight forward:
1. Run `sudo raspi-config` in the terminal
2. Under "Interface and Serial" disable the login shell and enable the serial port hardware
3. Edit `/boot/comfig.txt` and add the following lines to the bottom of the file:
    *`dtoverlay=pi3-miniuart-bt`
    *`enable_uart=1`
4. Reboot