#include <kern.h>
#include <bwio.h>
#include <dump.h>

void exitKernel(void* processStackPtr){

    // save kernel registers on kernel stack
    asm("stmfd sp!, {r0-r12, lr}");

    asm("ldr r1, [r0]");
    asm("msr cpsr_c, r1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("add sp, r0, #4");

    //loads user mode registers
    //includes the PC register, and starts executing
    asm("ldmfd SP, {r0-pc}");

    bwprintf(COM2, "end of exitKernel\r\n");
}

void enterKernel(){
    asm("LDMFD SP!, {R0-R12, LR}");
    asm("MOV PC, LR");
}
