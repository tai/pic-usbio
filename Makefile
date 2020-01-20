
CPU = 18F14K50
#CPU = 16F1459

USBIO_VID = 0x0BFE
USBIO_PID = 0x1000

CC = /opt/microchip/xc8/v2.10/bin/xc8-cc -mcpu=$(CPU)
LD = $(CC)

TARGET = firmware.elf
TOPDIR = .

# Either download whole MLA or clone MLA-USB repository
# - https://github.com/MicrochipTech/mla_usb.git
#MLAUSB = /opt/microchip/mla/v2018_11_26/framework/usb
MLAUSB = /d/src/pic/mla_usb

CDEFS  = \
	-I$(TOPDIR)/src -I$(MLAUSB)/inc \
	-DUSBIO_VID=$(USBIO_VID) -DUSBIO_PID=$(USBIO_PID)

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
	$(MLAUSB)/src/usb_device.c \
	$(MLAUSB)/src/usb_device_hid.c

SRCS = $(notdir $(TOP_SRC)) $(notdir $(MLA_SRC))
OBJS = $(SRCS:.c=.p1)

VPATH = $(dir $(TOP_SRC) $(MLA_SRC))

# NOTE: Beware if powering the chip from USB
#PK2CMD = pk2cmd -A3.3 -T -P
PK2CMD = pk2cmd -P

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
	$(PK2CMD) -R -M -F build/$(TARGET:.elf=.hex)

upload-mplab:
	$(PK2CMD) -R -M -F dist/*/*/*.hex

reset:
	$(PK2CMD) -R

clean:
	$(RM) *.d *.i *.p1 *.old *.bak *~
	$(RM) -r build

