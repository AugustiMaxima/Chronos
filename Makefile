#
# Makefile for busy-wait IO library
#
XDIR=/u/cs452/public/gnuarm-4.0.2
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-elf/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-elf/4.0.2
CC = $(XBINDIR)/arm-elf-gcc
AS = $(XBINDIR)/arm-elf-as
LD = $(XBINDIR)/arm-elf-ld

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CFLAGS = -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I include

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T linker.ld -L lib -L $(XLIBDIR2)

all: kernel.elf

kernel.s: kernel.c
	$(CC) -S $(CFLAGS) kernel.c

kernel.o: kernel.s
	$(AS) $(ASFLAGS) -o kernel.o kernel.s

kernel.elf: kernel.o
	$(LD) $(LDFLAGS) -o $@ kernel.o -lbwio -lgcc

clean:
	-rm -f kernel.elf *.s *.o
