 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>

void __attribute__((naked)) handle_swi () {
    bwprintf(COM2, "handle_swi\r\n");
    printCurrentMode();
    for (;;) {}
}

void* yield() {
    bwprintf(COM2, "yielding\r\n");
    // some assembly code here to context switch back to the kernel
    asm("swi");
}

void* first() {
    bwprintf(COM2, "First task\r\n");
    printCurrentMode();
}

void* __attribute__((naked)) call_user_task() {

    first();
    yield();
}

// void* enter_kernel() {
//     bwprintf(COM2, "enter_kernel\r\n");
// }

void* exitKernel() {
    bwprintf(COM2, "exitKernel\r\n");

    // save kernel registers
    asm volatile("stmfd sp!, {r0-r12, lr}");

    // set user mode
    asm volatile("mrs r3, cpsr");
    asm volatile("bic r3, r3, #0x1F");
    asm volatile("orr r3, r3, #0x10");
    asm volatile("msr cpsr_c, r3");

    // give user some stack space
    asm volatile("mov sp, #0x0070000");

    asm volatile("bl call_user_task");
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

int main( int argc, char* argv[] ) {

	bwsetfifo(COM2, OFF);

    printCurrentMode();

    /*
    Install SWI handler
    The swi instruction executes at address 0x8. The following two lines write
    this to the memory addresses:

    0x08        LDR pc, [pc, #0]
    0x0c        ?
    0x10        <absolute address of handle_swi>

    Note that ARM prefetches 2 instructions ahead. Hence, after a software
    interrupt, instruction 0x08 executes with pc=0x10.
    */
    *((unsigned*)0x8) = 0xe59ff000;
    *((unsigned*)0x10) = handle_swi;



    // kernel loop
    exitKernel();

    bwprintf(COM2, "after exitKernel\r\n");

	return 0;
}
