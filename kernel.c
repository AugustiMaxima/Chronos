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

void handleInterrupt() {
    bwprintf(COM2, "handleInterrupt\r\n");
    bwprintf(COM2, "VIC1IRQStatus=0x%x\r\n", *(int*)VIC1_BASE);

}

void installInterruptHandler(void* handler){
    /*
    Install Interrupt handler

    0x18        LDR pc, [pc, #0x18]
    0x0c        ?
    ...         ?
    0x38        <absolute address of handle_Interrupt>

    */
    *(unsigned*)0x18 = 0xe59ff018;
    *(unsigned*)0x38 = handler;
}

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

    scheduleTask(scheduler, 0, 0, k2_rps_main);

    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            break;
        }
    }

    return 0;

    installInterruptHandler(handleInterrupt);

    *(int*)(VIC1_BASE + 0x10) = 0x10;
    enableInterrupts();

    initializeTimer(1, 2000, 0x1ffe, 0);

    for (int i=0;;i++) {
        bwprintf(COM2, "i=%d\tvalue=%d\tIntr=%d\r\n", i, getValue(1), *(int*)(VIC1_BASE));
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
	    if (node && receiver->tId != getNsTid()) bwprintf(COM2, "Warning: TID %d blocked on receive\r\n", receiver->tId);
    } while(node!=0);

    node = 0;
    do {
	    node = iterateMap(&(com->senderReplyTable), node);
        Receiver* receiver = (Receiver*) node;
	    if (node) bwprintf(COM2, "Warning: TID %d blocked because it executed a send but target task has not called reply\r\n", receiver->tId);
    } while(node!=0);

    return 0;
}
