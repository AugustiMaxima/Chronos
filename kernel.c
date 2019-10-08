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
#include <idle.h>
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
    enableDevice(0x10, 0x0);

    hypeTrain();
    Scheduler base_scheduler;
    scheduler = &base_scheduler;

    COMM commRelay;
    com = &commRelay;

    DeviceRegistry deviceRegistry;
    registry = &deviceRegistry;

    Clock clock;

    initializeScheduler(scheduler);
    initializeCOMM(com);
    initializeDeviceRegistry(registry);

    initializeClock(&clock, 3, 508000, 0, 0, 0, 0);

    //scheduleTask(scheduler, 0, 0, ssr_test_main);

    scheduleTask(scheduler, 0, 0, k2_rps_main);

    // initializeTimer(1, 508000, 5080, 1);
    // scheduleTask(scheduler, 0, 0, k3_main);

    unsigned long last = 0;
    unsigned long utilTime = 0;
    unsigned long begin;
    unsigned long end;
    unsigned long rate;

    Task idler;

    initializeTask(&idler, -1, 0, -1, HALTED, idle);

    //scheduleTask(scheduler, 0,0, heapTest);
    while(1){
	    timeElapsed(&clock);
        begin = getOscilation(&clock);
        while(1) {
            if (-1 == runFirstAvailableTask(scheduler)) {
                break;
            }
        }
        //assumption: kernel isn't busy continuously for 2 hours
        //if it ever gets that busy, move this into the small loop
        timeElapsed(&clock);
        end = getOscilation(&clock);
        utilTime+=end - begin;
        //bwprintf(COM2, "%d\r\n", end);
        if(end - last > 508000){//only polls for changes in the last 100 miliseconds
            rate = (end - last - utilTime)*1000/((unsigned)end - (unsigned)last);
            bwprintf(COM2, "Utilized time %d / %d\r\n", end - begin, end - last);
            utilTime = 0;
            last = end;
            bwprintf(COM2, "Utilization time: %d", rate/100);
            bwprintf(COM2, "%d", rate%100/10);
            bwprintf(COM2, ".%d\r\n", rate%10);
        }
	    scheduler->currentTask = &idler;
	    exitKernel(idler.stackEntry);
    }
    disableTimer();

    // set all ICU masks off
    enableDevice(0x0, 0x0);

    warnAtEndOfKernel(com);

    return 0;
}
