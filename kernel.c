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
#include <k3.h>
#include <clock.h>
#include <timer.h>
#include <maptest.h>
#include <interrupt.h>
#include <deviceRegistry.h>
#include <dump.h>

Scheduler* scheduler;
COMM* com;
DeviceRegistry* registry;

// each task-local seed is derived as seed = seedSeed + MyTid()
const unsigned seedSeed = 0xdeadbeef;

int nsTid = -1;

int main( int argc, char* argv[] ) {
    bwsetfifo(COM2, OFF);
    setUpSWIHandler(sys_handler);
    installInterruptHandler(interruptHandler);
    enableDevice(0x0, 0x80000);

    hypeTrain();
    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    DeviceRegistry deviceRegistry;
    registry = &deviceRegistry;

    initializeScheduler(scheduler);
    initializeCOMM(com);
    initializeDeviceRegistry(registry);

    initializeTimer(3, 2000, 200, 1);

    //scheduleTask(scheduler, 0, 0, ssr_test_main);

    scheduleTask(scheduler, 0, 0, k3_main);


    while(1) {
        if (-1 == runFirstAvailableTask(scheduler)) {
            break;
        }
    }

    disableTimer();

    // set all ICU masks off
    enableDevice(0x0, 0x0);

    warnAtEndOfKernel(com);

    return 0;
}
