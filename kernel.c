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

    initializeTimer(1, 508000, 5080, 1);
    initializeClock(&clock, 3, 508000, 0, 0, 0, 0);

    //scheduleTask(scheduler, 0, 0, ssr_test_main);

    // scheduleTask(scheduler, 0, 0, k2_rps_main);

    scheduleTask(scheduler, 0, 0, k3_main);

    unsigned utilTime = 0;
    unsigned totalUtilTime = 0;
    unsigned fullTime = 0;
    unsigned lastFullTime = 0;
    unsigned lastUtilTime = 0;
    TimeStamp epoch;
    TimeStamp begin;
    TimeStamp end;

    Task idler;

    initializeTimeStamp(&epoch, 0, 0, 0, 0);
    initializeTask(&idler, -1, 0, -1, HALTED, idle);

    //scheduleTask(scheduler, 0,0, heapTest);
    while(1){
	timeElapsed(&clock);
        getCurrentTime(&clock, &begin);
        while(1) {
            if (-1 == runFirstAvailableTask(scheduler)) {
                break;
            }
        }
        //assumption: kernel isn't busy continuously for 2 hours
        //if it ever gets that busy, move this into the small loop
        timeElapsed(&clock);
        getCurrentTime(&clock, &end);
        utilTime = compareTime(&end, &begin);
        totalUtilTime += utilTime;
        fullTime = compareTime(&end, &epoch);
	if(fullTime - lastFullTime > 500){//only polls for changes in the last 500 miliseconds
	    utilTime = (fullTime - lastFullTime - totalUtilTime + lastUtilTime)*100000/(fullTime - lastFullTime);
	    lastUtilTime = totalUtilTime;
	    lastFullTime = fullTime;
	    bwprintf(COM2, "Utilization time: %d", utilTime/10000);
	    bwprintf(COM2, "%d", utilTime%10000/1000);
	    bwprintf(COM2, ".%d\r\n", utilTime%1000);
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
