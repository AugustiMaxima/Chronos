#include <bwio.h>
#include <ts7200.h>

void printSp() {
    unsigned int sp;
    asm volatile("mov %0, sp" : "=r" (sp));

    bwputstr(COM2, "sp=");
    bwputr(COM2, sp);
    bwputstr(COM2, "\r\n");
}

void printCurrentMode() {
    unsigned int cpsr;
    asm volatile("mrs %0, cpsr" : "=r" (cpsr));

    cpsr = cpsr & 0x1F; // get last 5 bits

    if (cpsr == 0x10) {
        bwprintf(COM2, "user mode\r\n");
    } else if (cpsr == 0x11) {
        bwprintf(COM2, "fiq mode\r\n");
    } else if (cpsr == 0x12) {
        bwprintf(COM2, "irq mode\r\n");
    } else if (cpsr == 0x13) {
        bwprintf(COM2, "supervisor mode\r\n");
    } else if (cpsr == 0x17) {
        bwprintf(COM2, "abort mode\r\n");
    } else if (cpsr == 0x1B) {
        bwprintf(COM2, "undefined mode\r\n");
    } else if (cpsr == 0x1F) {
        bwprintf(COM2, "system mode\r\n");;
    } else {
        // we read something in the mode bits that's not supposed to be there
        bwprintf(COM2, "illegal mode\r\n");
    }
}

void installSwiHandler(void* handle_swi) {
    /*
    Install SWI handler
    The swi instruction executes at address 0x8. The following two lines write
    this to the memory addresses:

    0x08        LDR pc, [pc, #0x18]
    0x0c        ?
    ...         ?
    0x28        <absolute address of handle_swi>

    Note that ARM prefetches 2 instructions ahead. Hence, after a software
    interrupt, instruction 0x08 executes with pc=0x10.
    */
    *((unsigned*)0x8) = 0xe59ff018;
    *((unsigned*)0x28) = handle_swi;
}