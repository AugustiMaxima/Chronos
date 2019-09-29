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
CFLAGS = -O3 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I include -I arch -I kern -I lib -I task -I user -I user/library -I util -I comm -I misc

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
LDFLAGS = -static -e main -nmagic -T linker.ld -L lib -L $(XLIBDIR2)

LIBS = -lbwio -ldump -larm -lscheduler -lsyscall -luserprogram -lpriorityQueue -lqueue -lkern -ltask -lsyslib -lmap -lsendReceiveReply -lgcc

all: kernel.elf

kernel.s: kernel.c
	$(CC) -S $(CFLAGS) kernel.c

kernel.o: kernel.s
	$(AS) $(ASFLAGS) -o kernel.o kernel.s

kernel.elf: kernel.o dump.a arm.a bwio.a scheduler.a syscall.a userprogram.a queue.a kern.a task.a priorityQueue.a syslib.a map.a sendReceiveReply.a
	$(LD) $(LDFLAGS) -o $@ kernel.o $(LIBS) $(LIBS)

dump.s: misc/dump.c
	$(CC) -S $(CFLAGS) misc/dump.c

dump.o: dump.s
	$(AS) $(ASFLAGS) -o dump.o dump.s

dump.a: dump.o
	$(AR) $(ARFLAGS) $@ dump.o

arm.s: arch/ARM.c
	$(CC) -S $(CFLAGS) arch/ARM.c -o arm.s

arm.o: arm.s
	$(AS) $(ASFLAGS) -o arm.o arm.s

arm.a: arm.o
	$(AR) $(ARFLAGS) $@ arm.o

bwio.s: misc/bwio.c
	$(CC) -S $(CFLAGS) misc/bwio.c

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

kern.s: kern/kern.c
	$(CC) -S $(CFLAGS) kern/kern.c

kern.o: kern.s
	$(AS) $(ASFLAGS) -o kern.o kern.s

kern.a: kern.o
	$(AR) $(ARFLAGS) $@ kern.o

syscall.s: kern/syscall.c
	$(CC) -S $(CFLAGS) kern/syscall.c

syscall.o: syscall.s
	$(AS) $(ASFLAGS) -o syscall.o syscall.s

syscall.a: syscall.o
	$(AR) $(ARFLAGS) $@ syscall.o

syslib.s: user/library/syslib.c
	$(CC) -S $(CFLAGS) user/library/syslib.c

syslib.o: syslib.s
	$(AS) $(ASFLAGS) -o syslib.o syslib.s

syslib.a: syslib.o
	$(AR) $(ARFLAGS) $@ syslib.o

userprogram.s: user/userprogram.c
	$(CC) -S $(CFLAGS) user/userprogram.c

userprogram.o: userprogram.s
	$(AS) $(ASFLAGS) -o userprogram.o userprogram.s

userprogram.a: userprogram.o
	$(AR) $(ARFLAGS) $@ userprogram.o

queue.s: util/queue.c
	$(CC) -S $(CFLAGS) util/queue.c

queue.o: queue.s
	$(AS) $(ASFLAGS) -o queue.o queue.s

queue.a: queue.o
	$(AR) $(ARFLAGS) $@ queue.o

priorityQueue.s: task/priorityQueue.c
	$(CC) -S $(CFLAGS) task/priorityQueue.c

priorityQueue.o: priorityQueue.s
	$(AS) $(ASFLAGS) -o priorityQueue.o priorityQueue.s

priorityQueue.a: priorityQueue.o
	$(AR) $(ARFLAGS) $@ priorityQueue.o

scheduler.s: task/scheduler.c
	$(CC) -S $(CFLAGS) task/scheduler.c

scheduler.o: scheduler.s
	$(AS) $(ASFLAGS) -o scheduler.o scheduler.s

scheduler.a: scheduler.o
	$(AR) $(ARFLAGS) $@ scheduler.o

map.s: util/map.c
	$(CC) -S $(CFLAGS) util/map.c

map.o: map.s
	$(AS) $(ASFLAGS) -o map.o map.s

map.a: map.o
	$(AR) $(ARFLAGS) $@ map.o

sendReceiveReply.s: comm/sendReceiveReply.c
	$(CC) -S $(CFLAGS) comm/sendReceiveReply.c

sendReceiveReply.o: sendReceiveReply.s
	$(AS) $(ASFLAGS) -o sendReceiveReply.o sendReceiveReply.s

sendReceiveReply.a: sendReceiveReply.o
	$(AR) $(ARFLAGS) $@ sendReceiveReply.o

.PHONY: install clean

clean:
	-rm -f kernel.elf *.s *.o *.a

install: kernel.elf
	-cp kernel.elf /u/cs452/tftp/ARM/$(shell whoami)
