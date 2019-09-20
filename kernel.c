 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>

void* first() {
    bwprintf("First task\r\n");
}

void* call_user_task() {
    first();
    // some assembly code here to context switch back to the kernel
}


void* leave_kernel() {
    bwprintf("leave_kernel\r\n");
    // some assembly code here to context switch to call_user_task
    enter_kernel();
}

void* enter_kernel() {
    bwprintf("enter_kernel\r\n");
}

int main( int argc, char* argv[] ) {

    leave_kernel();

	return 0;
}
