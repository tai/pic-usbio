
#CPU = 18F14K50
CPU = 16F1459

CC = /opt/microchip/xc8/v2.10/bin/xc8-cc -mcpu=$(CPU)
LD = $(CC)

TARGET = firmware.elf
TOPDIR = .

#MLADIR = /opt/microchip/mla/v2018_11_26
MLADIR = $(TOPDIR)

CDEFS  = \
	-I$(TOPDIR)/src -I$(MLADIR)/framework/usb/inc

CFLAGS = \
	-std=c90 -xassembler-with-cpp -gdwarf-3 \
	-fno-short-double -fno-short-float \
	-Wa,-a \
	-mno-keep-startup -mno-download -mdefault-config-bits \
	-memi=wordwrite -maddrqual=ignore \
	-mstack=compiled:auto:auto:auto \
	-msummary=-psect,-class,+mem,-hex,-file

LDFLAGS = \
	-ginhx032 -Wl,--data-init -Wl,--defsym=__MPLAB_BUILD=1

TOP_SRC = $(wildcard $(TOPDIR)/src/*.c)
MLA_SRC = \
	$(MLADIR)/framework/usb/src/usb_device.c \
	$(MLADIR)/framework/usb/src/usb_device_hid.c

SRCS = $(notdir $(TOP_SRC)) $(notdir $(MLA_SRC))
OBJS = $(SRCS:.c=.p1)

VPATH = $(dir $(TOP_SRC) $(MLA_SRC))

.SUFFIXES:
.SUFFIXES: .elf .hex .p1 .c .h

.c.p1:
	$(CC) -c $(CFLAGS) $(CDEFS) -o $@ $<

all:
	@echo "Type: make (build|upload|clean)"

$(TARGET): $(OBJS)
	$(LD) -Wl,-Map=$*.map -Wl,--memorysummary,$*.xml -o $@ $(OBJS)

.PHONY: build
build:
	mkdir -p build
	$(MAKE) -C build -f ../Makefile TOPDIR=.. $(TARGET)

upload: build
	pk2cmd -P -T -R -M -F build/$(TARGET:.elf=.hex)

upload-mplab:
	pk2cmd -P -T -R -M -F dist/*/*/*.hex

reset:
	pk2cmd -P -T -R

clean:
	$(RM) *.d *.i *.p1 *.old *.bak *~
	$(RM) -r build
