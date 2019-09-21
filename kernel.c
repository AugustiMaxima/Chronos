 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>

void __attribute__((naked)) handle_swi () {
    bwprintf(COM2, "handle_swi\r\n");
    for (;;) {}
}

void* yield() {
    bwprintf(COM2, "yielding\r\n");
    // some assembly code here to context switch back to the kernel
    asm("swi");
}

void* first() {
    bwprintf(COM2, "First task\r\n");
}

void* call_user_task() {
    first();
    yield();
}

// void* enter_kernel() {
//     bwprintf(COM2, "enter_kernel\r\n");
// }

void* leave_kernel() {
    bwprintf(COM2, "leave_kernel\r\n");

    // save kernel registers
    asm("stmfd sp!, {r0-r12, lr}");

    // some assembly code here to context switch to call_user_task
    asm("mov pc, %[target]" :: [target] "r" (call_user_task));
}

int main( int argc, char* argv[] ) {

	bwsetfifo(COM2, OFF);

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
    leave_kernel();

	return 0;
}
