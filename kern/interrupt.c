#include <ARM.h>
#include <syscode.h>
#include <interrupt.h>



//Pattern:
//Avoid stacks and any variable statements
//Use a separate stackful function to deal with anything that requires stack manipulation
//comfy and safe
void interruptProcessor(){
    

}



void __attribute__((naked)) interruptHandler(){

    //BLOCK 1:
    //saving context as system mode

    //save the LR - 4 first as this is our reentry    
    asm("SUB SP, SP, #4");
    asm("SUB LR, LR, #4");
    asm("STR LR, [SP]");

    asm("MSR CPSR_c"TOSTRING(SYS_MODE))

    asm("STMFD SP!, {R0-R12, R14-R15}");
 
    asm("MSR CPSR_c"TOSTRING(IRQ_MODE));

    //handles actual processing
    asm("BL interruptProcessor");

    //restores the corrected lr
    //primed for arguments
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");
    
    //switches to svc mode from irq
    asm("MSR CPSR_c"TOSTRING(SVC_MODE));
    
    //handles restoration
    asm("BL handleSuspendedTasks");

    //returns
    asm("B enterKernel");
}
