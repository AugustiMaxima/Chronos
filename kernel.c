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

void* exitKernel() {
    bwprintf(COM2, "exitKernel\r\n");

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

    Scheduler scheduler;
    intializeScheduler(&scheduler);

	bwsetfifo(COM2, OFF);
    installSwiHandler(handle_swi);

    scheduleTask(&scheduler, 0, 0, call_user_task);

    struct os_Task t1;
    initializeTask(&t1, 0, 0, 0);

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

    int i = CPSR_M_SVC;

    // kernel loop
    exitKernel();

    bwprintf(COM2, "after exitKernel\r\n");

	return 0;
}
