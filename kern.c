#include "kern.h"

void exitKernel(void* usrsp){
    asm("STMFD sp!, {r0-r12, lr}");

    //change to system mode
    asm("LDR r1, [r0]");
    //load, increment after
    asm("ADD r0, r0, #4");
    //we are now in system mode
    asm("MSR CPSR_c, r1");
    
    //loads user mode registers
    //includes the PC register, and starts executing
    asm("LDMFD r0!, {r0-pc}");
    //note: requires syscall to be careful of where pc needs to go after execution
    //should work well for interrupt
}

void enterKernel(){
    asm("LDMFD sp!, {r0-r12, lr}");
    asm("mov pc, lr");
}