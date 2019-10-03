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
#include <ssrTest.h>
#include <k1.h>
#include <k2.h>
#include <clock.h>
#include <timer.h>
#include <maptest.h>;

Scheduler* scheduler;
COMM* com;
// each task-local seed is derived as seed = seedSeed + MyTid()
const unsigned seedSeed = 0xdeadbeef;

int nsTid = -1;

int main( int argc, char* argv[] ) {
    bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);

    //hypeTrain();
    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    initializeScheduler(scheduler);
    initializeCOMM(com);

    Clock clock;
    initializeClock(&clock, 3, 508000, 0,0,0,0);

    // scheduleTask(scheduler, 10, 0, nameServer);
    // scheduleTask(scheduler, 0, 0, NameServerTest);
    // scheduleTask(scheduler, 0, 0, k2_rps_main);

    // runFirstAvailableTask(scheduler);
    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }
    // scheduleTask(scheduler, 1, 0, SendReceive4);

    scheduleTask(scheduler, 1, 0, DynamoTest);

    TimeStamp begin;
    TimeStamp finish;

    // getCurrentTime(&clock, &begin);

    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }

    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "SendReceive4: %dms\r\n", compareTime(&finish, &begin));

    // scheduleTask(scheduler, 1, 0, ReceiveSend4);
    
    getCurrentTime(&clock, &begin);

    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            break;
        }
    }
    getCurrentTime(&clock, &finish);
    bwprintf(COM2, "ReceiveSend4: %dms\r\n", compareTime(&finish, &begin));

    // scheduleTask(scheduler, 1, 0, SendReceive64);
    // getCurrentTime(&clock, &begin);
    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }
    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "SendReceive64: %dms\r\n", compareTime(&finish, &begin));

    // scheduleTask(scheduler, 1, 0, ReceiveSend64);
    // getCurrentTime(&clock, &begin);
    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }
    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "ReceiveSend64: %dms\r\n", compareTime(&finish, &begin));

    // scheduleTask(scheduler, 1, 0, SendReceive256);
    // getCurrentTime(&clock, &begin);
    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }
    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "SendReceive256: %dms\r\n", compareTime(&finish, &begin));

    // scheduleTask(scheduler, 1, 0, ReceiveSend256);
    // getCurrentTime(&clock, &begin);
    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }
    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "ReceiveSend256: %dms\r\n", compareTime(&finish, &begin));

    if (0 != com->senderRequestTable.root) {
        bwprintf(COM2, "\r\nwarning: kernel exiting with senders blocked on receive\r\n");
    }
    if (0 != com->receiverTable.root) {
        bwprintf(COM2, "\r\nwarning: kernel exiting with receivers blocked\r\n");
    }
    if (0 != com->senderReplyTable.root) {
        bwprintf(COM2, "\r\nwarning: kernel exiting with senders blocked on reply\r\n");
    }

    return 0;
}
