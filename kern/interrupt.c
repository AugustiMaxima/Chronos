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
   
    //now that LR is reliably saved
    //use LR as a scratch register
    asm("MRS R14, CPSR");
    //switches to sysmode
    asm("ADD R14, R14, #13");
    asm("MSR CPSR, R14");

    asm("STMFD SP!, ")
 
    //handles actual processing
    asm("BL interruptProcessor");

    //restores the corrected lr
    //primed for arguments
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");
    
    //switches to svc mode from irq
    asm("MRS R2, CPSR");
    asm("ADD R2, R2, #1");
    asm("MSR CPSR, R2");
    
    //handles restoration
    asm("BL handleSuspendedTasks");

    //returns
    asm("B enterKernel");
}
