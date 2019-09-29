 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <ts7200.h>
#include <ARM.h>
#include <syscall.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <userprogram.h>
#include <bwio.h>

Scheduler* scheduler;
COMM* com;

int main( int argc, char* argv[] ) {

    bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);

    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    initializeScheduler(scheduler);
    initializeCOMM(com);

    // scheduleTask(scheduler, 0, 0, userCall);
    
    // scheduleTask(scheduler, 0, 0, MapTest);


    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            return;
        }
    }

    return 0;
}
