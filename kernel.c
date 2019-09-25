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

    bwprintf(COM2, "Scheduler Ptr: %x\r\n", scheduler);

    initializeScheduler(scheduler);

    scheduleTask(scheduler, 0, 0, userCall);

    runFirstAvailableTask(scheduler);

    bwprintf(COM2, "%s\r\n", "Back!");

    runFirstAvailableTask(scheduler);

	return 0;
}
