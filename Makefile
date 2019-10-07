#
# Makefile for busy-wait IO library
#

ifeq "useNew" "useNew"

XDIR =/u/cs452/public/xdev/
XBINDIR=$(XDIR)bin
XLIBDIR1=$(XDIR)arm-none-eabi/lib
XLIBDIR2=$(XDIR)lib/gcc/arm-none-eabi/9.2.0
CC = $(XBINDIR)/arm-none-eabi-gcc
AS = $(XBINDIR)/arm-none-eabi-as
LD = $(XBINDIR)/arm-none-eabi-ld

else

XDIR=/u/cs452/public/gnuarm-4.0.2
XBINDIR=$(XDIR)/bin
XLIBDIR1=$(XDIR)/arm-elf/lib
XLIBDIR2=$(XDIR)/lib/gcc/arm-elf/4.0.2
CC = $(XBINDIR)/arm-elf-gcc
AS = $(XBINDIR)/arm-elf-as
LD = $(XBINDIR)/arm-elf-ld

endif


ASFLAGS = -mfloat-abi=soft

# -g: include debug information for gdb
# -S: only compile and emit assembly
# -fPIC: emit position-independent code
# -Wall: report all warnings
# -mcpu=arm920t: generate code for the 920t architecture
# -msoft-float: no FP co-processor
CFLAGS = -std=gnu99 -O0 -g -S -fPIC -Wall -mcpu=arm920t -msoft-float -I. -I include -I arch -I kern -I lib -I task -I user -I user/library -I util -I comm -I misc -I test -I devices -I resource

# -static: force static linking
# -e: set entry point
# -nmagic: no page alignment
# -T: use linker script
#LDFLAGS = -static -e main -nmagic -T linker.ld -L lib -L $(XLIBDIR2)
LDFLAGS = -static -e main -nmagic -T linker.ld -L lib -L ../inc -L $(XLIBDIR1) -L $(XLIBDIR2) -lc

LIBS = -lbwio -ldump -larm -lscheduler -lsyscall -luserprogram -lpriorityQueue -lqueue -lkern -ltask -lsyslib -lmap -lsendReceiveReply -lmaptest -lk1 -lk2 -lk3 -lnameServer -lcharay -ltimer -lclock -lssrTest -lchlib -linterrupt -ldeviceRegistry -lminHeap -lclockServer -lgcc

all: kernel.elf

kernel.s: kernel.c
	$(CC) -S $(CFLAGS) kernel.c

kernel.o: kernel.s
	$(AS) $(ASFLAGS) -o kernel.o kernel.s

kernel.elf: kernel.o dump.a arm.a bwio.a clock.a scheduler.a syscall.a ssrTest.a timer.a userprogram.a queue.a kern.a task.a priorityQueue.a syslib.a map.a sendReceiveReply.a charay.a nameServer.a maptest.a k1.a k2.a k3.a chlib.a interrupt.a deviceRegistry.a minHeap.a clockServer.a
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

maptest.s: test/maptest.c
	$(CC) -S $(CFLAGS) test/maptest.c -o maptest.s

maptest.o: maptest.s
	$(AS) $(ASFLAGS) -o maptest.o maptest.s

maptest.a: maptest.o
	$(AR) $(ARFLAGS) $@ maptest.o

k1.s: test/k1.c
	$(CC) -S $(CFLAGS) test/k1.c -o k1.s

k1.o: k1.s
	$(AS) $(ASFLAGS) -o k1.o k1.s

k1.a: k1.o
	$(AR) $(ARFLAGS) $@ k1.o

k2.s: test/k2.c
	$(CC) -S $(CFLAGS) test/k2.c -o k2.s

k2.o: k2.s
	$(AS) $(ASFLAGS) -o k2.o k2.s

k2.a: k2.o
	$(AR) $(ARFLAGS) $@ k2.o

k3.s: test/k3.c
	$(CC) -S $(CFLAGS) test/k3.c -o k3.s

k3.o: k3.s
	$(AS) $(ASFLAGS) -o k3.o k3.s

k3.a: k3.o
	$(AR) $(ARFLAGS) $@ k3.o

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

interrupt.s: kern/interrupt.c
	$(CC) -S $(CFLAGS) kern/interrupt.c

interrupt.o: interrupt.s
	$(AS) $(ASFLAGS) -o interrupt.o interrupt.s

interrupt.a: interrupt.o
	$(AR) $(ARFLAGS) $@ interrupt.o

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

chlib.s: util/chlib.c
	$(CC) -S $(CFLAGS) util/chlib.c

chlib.o: chlib.s
	$(AS) $(ASFLAGS) -o chlib.o chlib.s

chlib.a: chlib.o
	$(AR) $(ARFLAGS) $@ chlib.o

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

minHeap.s: util/minHeap.c
	$(CC) -S $(CFLAGS) util/minHeap.c

minHeap.o: minHeap.s
	$(AS) $(ASFLAGS) -o minHeap.o minHeap.s

minHeap.a: minHeap.o
	$(AR) $(ARFLAGS) $@ minHeap.o

charay.s: util/charay.c
	$(CC) -S $(CFLAGS) util/charay.c

charay.o: charay.s
	$(AS) $(ASFLAGS) -o charay.o charay.s

charay.a: charay.o
	$(AR) $(ARFLAGS) $@ charay.o

sendReceiveReply.s: comm/sendReceiveReply.c
	$(CC) -S $(CFLAGS) comm/sendReceiveReply.c

sendReceiveReply.o: sendReceiveReply.s
	$(AS) $(ASFLAGS) -o sendReceiveReply.o sendReceiveReply.s

sendReceiveReply.a: sendReceiveReply.o
	$(AR) $(ARFLAGS) $@ sendReceiveReply.o

nameServer.s: user/library/nameServer.c
	$(CC) -S $(CFLAGS) user/library/nameServer.c

nameServer.o: nameServer.s
	$(AS) $(ASFLAGS) -o nameServer.o nameServer.s

nameServer.a: nameServer.o
	$(AR) $(ARFLAGS) $@ nameServer.o

clockServer.s: user/library/clockServer.c
	$(CC) -S $(CFLAGS) user/library/clockServer.c

clockServer.o: clockServer.s
	$(AS) $(ASFLAGS) -o clockServer.o clockServer.s

clockServer.a: clockServer.o
	$(AR) $(ARFLAGS) $@ clockServer.o

timer.s: devices/timer.c
	$(CC) -S $(CFLAGS) devices/timer.c

timer.o: timer.s
	$(AS) $(ASFLAGS) -o timer.o timer.s

timer.a: timer.o
	$(AR) $(ARFLAGS) $@ timer.o

deviceRegistry.s: devices/deviceRegistry.c
	$(CC) -S $(CFLAGS) devices/deviceRegistry.c

deviceRegistry.o: deviceRegistry.s
	$(AS) $(ASFLAGS) -o deviceRegistry.o deviceRegistry.s

deviceRegistry.a: deviceRegistry.o
	$(AR) $(ARFLAGS) $@ deviceRegistry.o

clock.s: resource/clock.c
	$(CC) -S $(CFLAGS) resource/clock.c

clock.o: clock.s
	$(AS) $(ASFLAGS) -o clock.o clock.s

clock.a: clock.o
	$(AR) $(ARFLAGS) $@ clock.o

ssrTest.s: test/ssrTest.c
	$(CC) -S $(CFLAGS) test/ssrTest.c

ssrTest.o: ssrTest.s
	$(AS) $(ASFLAGS) -o ssrTest.o ssrTest.s

ssrTest.a: ssrTest.o
	$(AR) $(ARFLAGS) $@ ssrTest.o

.PHONY: install clean

clean:
	-rm -f kernel.elf *.s *.o *.a

install: kernel.elf
	-cp kernel.elf /u/cs452/tftp/ARM/$(shell whoami)
