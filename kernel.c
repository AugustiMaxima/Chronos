 /*
 * kernel.c
 */

//ARCH
#include <ts7200.h>
#include <ARM.h>

//Kernel
#include <kern.h>
#include <syscall.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <interrupt.h>
#include <deviceRegistry.h>
#include <idle.h>

//Devices and abstractions
#include <clock.h>
#include <timer.h>
#include <uart.h>

//User programs
#include <userprogram.h>
#include <bwio.h>
#include <ssrTest.h>
#include <k1.h>
#include <k2.h>
#include <k3.h>
#include <k4.h>
#include <maptest.h>
#include <dump.h>
#include <deviceTests.h>

//Application
#include <control.h>

Scheduler* scheduler;
COMM* com;
DeviceRegistry* registry;

// each task-local seed is derived as seed = seedSeed + MyTid()
const unsigned seedSeed = 0xdeadbeef;

int nsTid = -1;

int kernelRunning = 1;

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

    initializeTimer(1, 2000, 20, 1); // 10ms
    setEnabledDevices((1 << TC1UI_DEV_ID), 0x0);
    
    Clock clock;
    initializeClock(&clock, 3, 508000, 0, 0, 0, 0);

    
    scheduleTask(scheduler, 0, 0, control);
    
    unsigned long last = 0;
    unsigned long utilTime = 0;
    unsigned long begin;
    unsigned long end;
    unsigned long rate;

    Task idler;
    initializeTask(&idler, -1, 0, -1, HALTED, idle);

    while(1) {
        timeElapsed(&clock);
        begin = getOscilation(&clock);
        if (!kernelRunning) {
            break;
        }
        while (-1 != runFirstAvailableTask(scheduler));
        timeElapsed(&clock);
        end = getOscilation(&clock);
        utilTime+=end - begin;
        if(end - last > 508000){//only polls for changes in the last 100 miliseconds
            rate = (end - last - utilTime)*1000/((unsigned)end - (unsigned)last);
            utilTime = 0;
            last = end;
	    // bwprintf(COM2, "%d\r\n", rate);
            //do display here
        }
	    scheduler->currentTask = &idler;
	    exitKernel(idler.stackEntry);
    }

    disableTimer();

    // set all ICU masks off
    setEnabledDevices(0x0, 0x0);

    warnAtEndOfKernel(com);

    return 0;
}
