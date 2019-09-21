#include "kern.h"

void exitKernel(void* processStackPtr){
    asm("stmfd sp!, {r0-r12, lr}");

    //change to system mode
    asm("ldr r1, [r0]");
    //we are now in system mode
    asm("msr CPSR_c, r1");

    //restores the stack pointer, minus the cpsr
    asm("sub sp, r0, #4");
    
    //loads user mode registers
    //includes the PC register, and starts executing
    asm("ldmfd sp!, {r0-pc}");
    //note: requires syscall to be careful of where pc needs to go after execution
    //should work well for interrupt
}

void enterKernel(){
    asm("ldmfd sp!, {r0-r12, lr}");
    asm("mov pc, lr");
}