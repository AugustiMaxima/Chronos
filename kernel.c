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

    scheduleTask(scheduler, 0, 0, ssr_test_main);

    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            break;
        }
    }

    volatile Node* node = 0;
    do {
	    node = iterateMap(&(com->senderRequestTable), node);
        Receiver* receiver = (Receiver*) node;
	    if (node) bwprintf(COM2, "Warning: TID %d blocked because it executed a send but target task has not called receive\r\n", receiver->tId);
    } while(node!=0);

    node = 0;
    do {
	    node = iterateMap(&(com->receiverTable), node);
        Receiver* receiver = (Receiver*) node;
	    if (node) bwprintf(COM2, "Warning: TID %d blocked on receive\r\n", receiver->tId);
    } while(node!=0);

    node = 0;
    do {
	    node = iterateMap(&(com->senderReplyTable), node);
        Receiver* receiver = (Receiver*) node;
	    if (node) bwprintf(COM2, "Warning: TID %d blocked because it executed a send but target task has not called reply\r\n", receiver->tId);
    } while(node!=0);

    return 0;
}
