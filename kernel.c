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
#include <k4.h>
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

int kernelRunning = 1;

int main( int argc, char* argv[] ) {
    bwsetfifo(COM1, OFF);
    bwsetfifo(COM2, OFF);
    bwsetspeed(COM1, 2400);
    bwsetstopbits(COM1, ON);

    setUpSWIHandler(sys_handler);
    installInterruptHandler(interruptHandler);

    // enable UART1 TX Interrupt and RX Interrupt
    int* uart1_ctrl = (int *)( UART1_BASE + UART_CTLR_OFFSET );
    *uart1_ctrl |= (TIEN_MASK | RIEN_MASK);

    // enable UART2 RX Interrupt
    int* uart2_ctrl = (int *)( UART2_BASE + UART_CTLR_OFFSET );
    *uart2_ctrl |= (TIEN_MASK | RIEN_MASK);

    setEnabledDevices(
        (1 << TC1UI_DEV_ID),
    0x0);

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

    initializeTimer(1, 2000, 20, 1); // 10ms
    scheduleTask(scheduler, 0, 0, k4_main);

    Task idler;
    initializeTask(&idler, -1, 0, -1, HALTED, idle);

    while(1) {
        if (!kernelRunning) {
            break;
        }
        if (-1 == runFirstAvailableTask(scheduler)) {
            scheduler->currentTask = &idler;
    	    exitKernel(idler.stackEntry);
        }
    }
    disableTimer();

    // set all ICU masks off
    setEnabledDevices(0x0, 0x0);

    warnAtEndOfKernel(com);

    return 0;
}
