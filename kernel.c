 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>
#include <scheduler.h>
#include <task.h>
#include <ARM.h>
#include <syscall.h>
#include <userprogram.h>

Scheduler* scheduler;
void* stackPtr;

int main( int argc, char* argv[] ) {

	bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);

    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    initializeScheduler(scheduler);

    // scheduleTask(scheduler, 0, 0, userCall);

    // int i;
    // for (i=0; i<50; i++) {
    //     runFirstAvailableTask(scheduler);
    // }

    // bwprintf(COM2, "size: %d\r\n", ringFill(&(scheduler->readyQueue)));

    scheduleTask(scheduler, 0, 0, magicExit);

    runFirstAvailableTask(scheduler);
    runFirstAvailableTask(scheduler);

	return 0;
}
