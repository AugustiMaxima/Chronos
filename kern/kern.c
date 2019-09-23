#include <kern.h>
#include <bwio.h>

void exitKernel(void* processStackPtr){
    // save kernel registers on kernel stack
    asm("stmfd sp!, {r0-r12, lr}");

    //change to user mode
    // cpsr <- *processStackPtr

    int i=0;
    int temp;
    for(i=0; i<17; i++){
        asm("ldr %1, [%0]", :"=r"(temp) :"r"(processStackPtr));
        ((int*)processStackPtr)--;
        bwprintf(COM2, "%x\r\n", temp);
    }

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