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
CFLAGS = -O0 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I include -I arch -I kern -I lib -I task -I user

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

kernel.elf: kernel.o dump.a bwio.a task.a
	$(LD) $(LDFLAGS) -o $@ kernel.o -lbwio -ldump -ltask -lgcc

dump.s: dump.c
	$(CC) -S $(CFLAGS) dump.c

dump.o: dump.s
	$(AS) $(ASFLAGS) -o dump.o dump.s

dump.a: dump.o
	$(AR) $(ARFLAGS) $@ dump.o

bwio.s: bwio.c
	$(CC) -S $(CFLAGS) bwio.c

bwio.o: bwio.s
	$(AS) $(ASFLAGS) -o bwio.o bwio.s

bwio.a: bwio.o
	$(AR) $(ARFLAGS) $@ bwio.o

task.s: task/task.c
	$(CC) -S $(CFLAGS) task/task.c

task.o: task.s
	$(AS) $(ASFLAGS) -o task.o task.s

task.a: task.o
	$(AR) $(ARFLAGS) $@ task.o

.PHONY: install clean

clean:
	-rm -f kernel.elf *.s *.o *.a

install: kernel.elf
	-cp kernel.elf /u/cs452/tftp/ARM/x538li
