#include <kern.h>
#include <bwio.h>

void exitKernel(void* processStackPtr){

    bwprintf(COM2, "Ride to ruin and the world's ending\r\n") ;   

    // save kernel registers on kernel stack
    asm("stmfd sp!, {r0-r12, lr}");

    //change to user mode
    // cpsr <- *processStackPtr

    asm("ldr r1, [r0]");
    asm("msr cpsr_c, r1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("sub sp, r0, #4");

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
