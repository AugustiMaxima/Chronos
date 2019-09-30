 /*
 * kernel.c
 */

#include <ts7200.h>
#include <ARM.h>
#include <syscall.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <userprogram.h>
#include <nameServer.h>
#include <bwio.h>

#include <maptest.h>
#include <k1.h>
#include <k2.h>

Scheduler* scheduler;
COMM* com;
// each task-local seed is derived as seed = seedSeed + MyTid()
const unsigned seedSeed = 0xdeadbeef;

int main( int argc, char* argv[] ) {
    bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);

    hypeTrain();
    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    initializeScheduler(scheduler);
    initializeCOMM(com);

    scheduleTask(scheduler, 10, 0, nameServer);
    scheduleTask(scheduler, 0, 0, NameServerTest);
    //scheduleTask(scheduler, 0, 0, k2_main);


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
