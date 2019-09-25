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

int main( int argc, char* argv[] ) {

	bwsetfifo(COM2, OFF);
    installSwiHandler(handle_swi);

    Scheduler scheduler;
    initializeScheduler(&scheduler);

    int k = scheduleTask(&scheduler, 0, 0, call_user_task);

    if(k==-2){
	    bwprintf(COM2, "You dun goofed");
    }

    bwprintf(COM2, "scheduleTask finished, calling runfirst available task\r\n");
    runFirstAvailableTask(&scheduler);

    bwprintf(COM2, "after runFirst\r\n");

	return 0;
}
