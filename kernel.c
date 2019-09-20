 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>

void* first() {
    bwprintf(COM2, "First task\r\n");
}

void* call_user_task() {
    first();
    // some assembly code here to context switch back to the kernel
}

void* enter_kernel() {
    bwprintf(COM2, "enter_kernel\r\n");
}

void* leave_kernel() {
    bwprintf(COM2, "leave_kernel\r\n");

    asm("stmfd sp!, {r0-r12, lr}");
    asm("ldmfd sp!, {r0-r12, lr}");

    // some assembly code here to context switch to call_user_task
    asm("mov pc, %[target]" :: [target] "r" (call_user_task));

    enter_kernel();
}

int main( int argc, char* argv[] ) {

    leave_kernel();

	return 0;
}
