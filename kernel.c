 /*
 * kernel.c
 */

//ARCH
#include <ts7200.h>
#include <ARM.h>

//Kernel
#include <kernel.h>
#include <kern.h>
#include <syscall.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <interrupt.h>
#include <deviceRegistry.h>
#include <idle.h>

//Devices and abstractions
#include <chlib.h>
#include <timer.h>
#include <uart.h>
#include <clock.h>
#include <terminal.h>
#include <uiHelper.h>

//User programs
#include <userprogram.h>
#include <ssrTest.h>
#include <k1.h>
#include <k2.h>
#include <k3.h>
#include <k4.h>
#include <tc1.h>
#include <tc2.h>
#include <maptest.h>
#include <dump.h>
#include <deviceTests.h>
#include <uiTest.h>
#include <bwio.h>

Scheduler* scheduler;
COMM* com;
DeviceRegistry* registry;
KernelMetaData* kernelData;

// each task-local seed is derived as seed = seedSeed + MyTid()
const unsigned seedSeed = 0xdeadbeef;

int nsTid = -1;

int kernelSwitch = 1;

int main( int argc, char* argv[] ) {

    setUpSWIHandler(sys_handler);
    installInterruptHandler(interruptHandler);

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

    initializeUART(1, 2400, false, false, true, true, true);
    initializeUART(2, 115200, true, true, true, true, true);

    initializeTimer(1, 508000, 5080, 1); // 10ms
    setEnabledDevices((1 << TC1UI_DEV_ID), 0x0);
    
    Clock clock;
    initializeClock(&clock, 3, 508000, 0, 0, 0, 0);
    
    scheduleTask(scheduler, 0, 0, tc2);
    // scheduleTask(scheduler, 0, 0, cursorTest);

    unsigned long last = 0;
    unsigned long utilTime = 0;
    unsigned long begin;
    unsigned long end;
    unsigned long rate;

    Task idler;
    initializeTask(&idler, -1, 0, -1, HALTED, idle);

    KernelMetaData metaData;
    kernelData = &metaData;

    while(kernelSwitch) {
        timeElapsed(&clock);
        begin = getOscilation(&clock);
        while (kernelSwitch && -1 != runFirstAvailableTask(scheduler));
        timeElapsed(&clock);
        end = getOscilation(&clock);
        utilTime+=end - begin;
        if(end - last > 508* 500){//only polls for changes in the last 100 miliseconds
            rate = (end - last - utilTime)/(((unsigned)end - (unsigned)last)/10000);
            utilTime = 0;
            last = end;
            metaData.utilizationRate = rate;
        }
	    scheduler->currentTask = &idler;
	    exitKernel(idler.stackEntry);
    }

    disableTimer();

    // set all ICU masks off
    setEnabledDevices(0x0, 0x0);

    // warnAtEndOfKernel(com);

    return 0;
}
