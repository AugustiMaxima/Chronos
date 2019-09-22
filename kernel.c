 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>
#include <dump.h>
#include <scheduler.h>
#include <task.h>
#include <ARM.h>

Scheduler* scheduler;

void __attribute__((naked)) handle_swi () {
    bwprintf(COM2, "handle_swi\r\n");

    // restore saved kernel registers from stack
    asm("ldmfd SP!, {R0-R12, LR}");
    asm("mov pc, lr");
}

void* yield() {
    bwprintf(COM2, "yielding\r\n");
    asm("swi");
}

void* first() {
    bwprintf(COM2, "First task\r\n");
    printCurrentMode();
    printSp();
}

void* __attribute__((naked)) call_user_task() {
    first();
    yield();
}

void* leave_kernel() {
    bwprintf(COM2, "leave_kernel\r\n");

    // save kernel registers on kernel stack
    asm volatile("stmfd sp!, {r0-r12, lr}");

    // set user mode
    asm volatile("mrs r3, cpsr");
    asm volatile("bic r3, r3, #0x1F");
    asm volatile("orr r3, r3, #0x10");
    asm volatile("msr cpsr_c, r3");

    // give user some stack space
    asm volatile("mov sp, #0x0070000");

    asm volatile("bl call_user_task");

    bwprintf(COM2, "after bl\r\n");
}

int main( int argc, char* argv[] ) {

	bwsetfifo(COM2, OFF);
    installSwiHandler(handle_swi);

    Scheduler scheduler;
    initializeScheduler(&scheduler);

    scheduleTask(&scheduler, 0, 0, call_user_task);

    runFirstAvailableTask(&scheduler);

    bwprintf(COM2, "after runFirst\r\n");



    // // kernel loop
    // leave_kernel();

    // bwprintf(COM2, "after leave_kernel\r\n");

	return 0;
}
