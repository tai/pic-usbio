# USB-controlled digital IO adapter on USB-PICs

## Description

This is an USB-controlled digital IO adapter based on USB-PICs such as PIC16F1459 and PIC18F14K50.
It can be controlled (peek and poke) by custom HID command, and protocol-compatible with now-defunct Morphy Planning's "USB-IO" adapter.
Also, Python-based usbio(1) tool is available to control it from command line.

Original Mophy USB-IO used USB VID=0x0BFE and PID=0x1000, with some variation in PID.
For this adapter, it can be set at build-time by changing the definition in Makefile.

## Install

```
$ vi Makefile
$ make build
$ make upload
```

You will at least need to edit followings:

- Target CPU (CPU=)
- Path to the compiler (CC=)
- Path to the MLA installed (MLAUSB=)

## Usage

Set and get pin state:
```
// Set state
$ usbio -P 0x1000 0b11001010
0x00CA
// Get state in binary
$ usbio -P 0x1000 -f bin
0b0000000011001010
// Get state in inverted binary
$ usbio -P 0x1000 -f bin -i
0b1111111100110100
```

Get and set single pin state (pin #0, other pins will stay as-is)
```
$ usbio -P 0x1000 -p 0
0
$ usbio -P 0x1000 -p 0 1
1
```

Separately-provided usbio(1) can be used to control this adapter (or any Morphy USB-IO compatible adapter).
It is available at https://github.com/tai/usbio

## Requirements

- USB-supported PIC (PIC16F1459 or PIC18F14K50, other PIC may work as well)
- Microchip MLA framework (either of below)
  - https://www.microchip.com/mplab/microchip-libraries-for-applications
  - https://github.com/MicrochipTech/mla_usb.git 
- PK2CMD (to program with PICKit2)
  - You will need modified version of PK2CMD to program recent USB-PICs with PICKit2
  - Other tool can be used, especially if you are using anything newer than PICKit2

## Pinout (PIC16F1459)

- 12 digital IOs (in MSB to LSB order)
  - RA5, RA4, RB6, RB4, RC7-RC0
  - Pin will go high when bit is set (1) in corresponding USB packet field
- RB5/RB7 for UART (reserved)
- RA0/RA1 for USB
- RC0/RC1/RA3 for ICSP

## Pinout (PIC18F14K50)

- 10 digital IOs (in MSB to LSB order)
  - RB6, RB4, RC7-RC0
- RB5/RB7 for UART (reserved)
- RA0/RA1 for USB
- RA0/RA1/RA3 for ICSP
