#include <kern.h>

void exitKernel(void* processStackPtr){
    asm("STMFD SP!, {R0-R12, LR}");

    //change to user mode
    asm("LDR R1, [R0]");
    //we are now in user mode
    asm("MSR CPSR_c, R1");

    //restores the stack pointer, minus the cpsr
    asm("SUB SP, R0, #4");
    
    //loads user mode registers
    //includes the PC register, and starts executing
    asm("LDMFD SP!, {R0-PC}");
    //note: requires syscall to be careful of where pc needs to go after execution
    //should work well for interrupt
}

void enterKernel(){
    asm("LDMFD SP!, {R0-R12, LR}");
    asm("MOV PC, LR");
}