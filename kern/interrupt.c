#include <ARM.h>
#include <syscode.h>
#include <interrupt.h>
#include <bwio.h>
#include <ts7200.h>
#include <timer.h>

//Pattern:
//Avoid stacks and any variable statements
//Use a separate stackful function to deal with anything that requires stack manipulation
//comfy and safe
void interruptProcessor(){
    int statusMask = *((unsigned*)VIC2ADDR);
    if(statusMask&&0x80000){
        bwprintf(COM2, "Hey! Watch!\r\n");
        *(volatile unsigned*)(TIMER3_BASE + CLR_OFFSET);
    }
}



void __attribute__((naked)) interruptHandler(){

    //BLOCK 1:
    //saving context as system mode

    asm("MSR CPSR_c, #"TOSTRING(SVC_MODE));

    //save the LR - 4 first as this is our reentry    
    asm("SUB SP, SP, #4");
    asm("SUB LR, LR, #4");
    asm("STR LR, [SP]");

    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));

    asm("STMFD SP!, {R0-R12, R14-R15}");
    asm("MRS R2, SPSR");
    asm("STMFD SP!, {R2}");

    asm("MSR CPSR_c, #"TOSTRING(SVC_MODE));
    //handles actual processing
    asm("BL interruptProcessor");

    //restores the corrected lr
    //primed for arguments
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");
    
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
    *(unsigned*)0x28 = interruptHandler;
}


void enableDevice(int deviceList1, int deviceList2){
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE_CLEAR) = 0;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE_CLEAR) = 0;
    *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE) = deviceList1;
    *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE) = deviceList2;
}