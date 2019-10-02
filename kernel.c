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

//
#include <ts7200.h>
#include <bwio.h>
#include <timer.h>

#define WRAP_16 0xffff;
#define WRAP_32 0xffffffff;
#define LOWER_MASK 0x0000ffff;

void* getTimerBase(int timer){
    switch (timer){
        case 1: return (void*)TIMER1_BASE;
        case 2: return (void*)TIMER2_BASE;
        case 3: return (void*)TIMER3_BASE;
    }
    return (void*)TIMER3_BASE;
}

unsigned int sanitizeLength(int timer, unsigned int length){
    if (timer!=3){
        length &= LOWER_MASK;
    }
    return length;
}

void setFrequency(int timer, int frequency) {
    int* CRTL = getTimerBase(timer) + CRTL_OFFSET;
    switch (frequency){
    case 508000:
        *CRTL |= CLKSEL_MASK;
        break;
    default:
        *CRTL &= ~CLKSEL_MASK;
    }
}

void setMode(int timer, int mode) {
    int* CRTL = getTimerBase(timer) + CRTL_OFFSET;
    switch (mode){
    case 1:
        *CRTL |= MODE_MASK;
    break;
    default:
        *CRTL &= ~MODE_MASK;
    }
}


void initializeTimer(int timer, int frequency, unsigned int length, int mode){
    void* BASE = getTimerBase(timer);
    *(int*)(BASE + CRTL_OFFSET) &= ~ENABLE_MASK;
    unsigned int sanitizedLength = sanitizeLength(timer, length);
    *(int*)(BASE + LDR_OFFSET) = sanitizedLength;
    setFrequency(timer, frequency);
    setMode(timer, mode);
    *(int*)(BASE + CRTL_OFFSET) |= ENABLE_MASK;
}

unsigned int getValue(int timer) {
    void* BASE = getTimerBase(timer);
    unsigned int value = *(int*)(BASE + VAL_OFFSET);
    if(timer!=3)
        value &= LOWER_MASK;
    return value;
}

unsigned int getWrap(int timer) {
    switch (timer){
    case 3:
        return WRAP_32;
    default:
        return WRAP_16;
    }
}
//

//

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
    scheduleTask(scheduler, 0, 0, k2_rps_main);

    runFirstAvailableTask(scheduler);
    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            break;
        }
    }
    // scheduleTask(scheduler, 1, 0, SendReceive4);

    // TimeStamp begin;
    // TimeStamp finish;

    // getCurrentTime(&clock, &begin);

    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }

    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "SendReceive4: %dms\r\n", compareTime(&finish, &begin));

    // scheduleTask(scheduler, 1, 0, ReceiveSend4);
    // getCurrentTime(&clock, &begin);
    // while(1) {
    //     if (-1 == runFirstAvailableTask(scheduler)) {
    //         break;
    //     }
    // }
    // getCurrentTime(&clock, &finish);
    // bwprintf(COM2, "ReceiveSend4: %dms\r\n", compareTime(&finish, &begin));

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

    if (
        (0 != com->senderRequestTable.root) ||
        (0 != com->receiverTable.root) ||
        (0 != com->senderReplyTable.root)
    ) {
        bwprintf(COM2, "\r\nwarning: kernel exiting with blocked tasks\r\n");
    }

    return 0;
}
