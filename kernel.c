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
#include <priorityQueue.h>

Scheduler* scheduler;

int main( int argc, char* argv[] ) {

    bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);

    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    initializeScheduler(scheduler);

    // scheduleTask(scheduler, 0, 0, userCall);
    
    scheduleTask(scheduler, 0, 0, MapTest);


    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            return;
        }
    }

    return 0;
}
