#include <ARM.h>
#include <ts7200.h>
#include <syscode.h>
#include <deviceRegistry.h>
#include <timer.h>
#include <uart.h>
#include <interrupt.h>
#include <dump.h>
#include <bwio.h>

extern DeviceRegistry* registry;

//Pattern:
//Avoid stacks and any variable statements
//Use a separate stackful function to deal with anything that requires stack manipulation
//comfy and safe
void interruptProcessor(){
    int statusMask1 = *((unsigned*)VIC1ADDR);
    int statusMask2 = *((unsigned*)VIC2ADDR);
    
    int interruptProcessed = 0;

    bwprintf(COM2, "%x %x\r\n", statusMask1, statusMask2);

    if (statusMask1 & (1 << TC1UI_DEV_ID)) {
        WakeForDevice(registry, TC1UI_DEV_ID, *(volatile unsigned*)(TIMER1_BASE + VAL_OFFSET));
        // clear the timer
        *(volatile unsigned*)(TIMER1_BASE + CLR_OFFSET) = 0;
        interruptProcessed++;
    }
    if (statusMask1 & (1 << TC2UI_DEV_ID)) {
        WakeForDevice(registry, TC2UI_DEV_ID, *(volatile unsigned*)(TIMER2_BASE + VAL_OFFSET));
        // clear the timer
        *(volatile unsigned*)(TIMER2_BASE + CLR_OFFSET) = 0;
        interruptProcessed++;
    }
    if (statusMask1 & (1 << UART1TX_DEV_ID)) {
        setTransmitInterrupt(1, false);
        WakeForDevice(registry, UART1TX_DEV_ID, /*unused*/0);
        interruptProcessed++;
    }
    if (statusMask1 & (1 << UART1RX_DEV_ID)) {
        setReceiveInterrupt(1, false);
        WakeForDevice(registry, UART1RX_DEV_ID, /*unused*/0);
        interruptProcessed++;
    }
    if (statusMask2 & (1 << (INT_UART1 - 32))) {
        WakeForDevice(registry, INT_UART1, *(volatile unsigned*)(UART1_BASE + UART_INTR_OFFSET));
        *(volatile unsigned*)(UART1_BASE + UART_INTR_OFFSET) = 0;
	interruptProcessed++;
    }
    if (statusMask1 & (1 << UART2RX_DEV_ID)) {
        setReceiveInterrupt(2, false);
        WakeForDevice(registry, UART2RX_DEV_ID, /*unused*/0);
        interruptProcessed++;
    }
    if (statusMask1 & (1 << UART2TX_DEV_ID)) {
        setTransmitInterrupt(2, false);
        WakeForDevice(registry, UART2TX_DEV_ID, /*unused*/0);
        interruptProcessed++;
    }
    if (statusMask2 & (1 << (INT_UART2 - 32))) {
        WakeForDevice(registry, INT_UART2, *(volatile unsigned*)(UART2_BASE + UART_INTR_OFFSET));
        *(volatile unsigned*)(UART2_BASE + UART_INTR_OFFSET) = 0;
	interruptProcessed++;
    }
    if(!interruptProcessed){
        bwprintf(COM2, "PANIC: Unexpected Interrupt\r\n");
        bwprintf(COM2, "Triggered Interrupts:\t%x\t%x\r\n", statusMask1, statusMask2);
        for (;;) {}
    }

}

void __attribute__((naked)) interruptHandler(){

    //BLOCK 1:
    //saving context as system mode

    //save the LR - 4 first as this is our reentry
    asm("SUB SP, SP, #4");
    asm("SUB LR, LR, #4");
    asm("STR LR, [SP]");

    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));

    asm("STMFD SP!, {R0-R12, R14-R15}");

    asm("MSR CPSR_c, #"TOSTRING(IRQ_MODE));
    asm("MRS R2, SPSR");
    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));
    asm("STMFD SP!, {R2}");

    asm("MSR CPSR_c, #"TOSTRING(IRQ_MODE));
    //handles actual processing
    asm("BL interruptProcessor");

    //restores the corrected lr
    //primed for arguments
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");

    asm("MSR CPSR_c, #"TOSTRING(SVC_MODE));

    //handles restoration
    asm("BL handleSuspendedTasks");

    //returns
    asm("B enterKernel");
}

void installInterruptHandler(void* handler){
    /*
    Install Interrupt handler
    The swi instruction executes at address 0x18. The following two lines write
    this to the memory addresses:

    0x18        LDR pc, [pc, #0x18]
    0x0c        ?
    ...         ?
    0x38        <absolute address of handle_Interrupt>

    */
    *(unsigned*)0x18 = 0xe59ff018;
    *(unsigned*)0x38 = (unsigned)handler;
}


void setEnabledDevices(unsigned deviceList1, unsigned deviceList2){
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE_CLEAR) = ~0;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE_CLEAR) = ~0;
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE) = deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE) = deviceList2;
}

void disableDevice(unsigned deviceList1, unsigned deviceList2) {
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE_CLEAR) = deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE_CLEAR) = deviceList2;
}

void enableDevice(unsigned deviceList1, unsigned deviceList2) {
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE) |= deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE) |= deviceList2;
}

void enableDeviceInterrupt(int deviceId){
    int deviceMask1 = 0;
    int deviceMask2 = 0;
    
    if(deviceId <= 31) {
	deviceMask1 = 1 << deviceId;
    } else {
	deviceMask2 = 1 << (deviceId - 32);
    }
    
    enableDevice(deviceMask1, deviceMask2);
}
