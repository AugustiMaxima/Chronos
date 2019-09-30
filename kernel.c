 /*
 * kernel.c
 */

#include <ts7200.h>
#include <ARM.h>
#include <syscall.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <userprogram.h>
#include <bwio.h>

#include <maptest.h>
#include <k1.h>

Scheduler* scheduler;
COMM* com;
const int seedSeed = 5;

int main( int argc, char* argv[] ) {
    hypeTrain();
    bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);

    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    initializeScheduler(scheduler);
    initializeCOMM(com);

    scheduleTask(scheduler, 0, 0, k1_main);


    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            break;
        }
    }

    if (
        (0 != com->senderRequestTable.root) ||
        (0 != com->receiverTable.root) ||
        (0 != com->senderReplyTable.root)
    ) {
        bwprintf(COM2, "\r\nwarning: kernel exiting with blocked tasks\r\n");
    }

    return 0;
}
