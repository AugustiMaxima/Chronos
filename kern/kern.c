#include <kern.h>
#include <bwio.h>
#include <dump.h>

__attribute__((naked)) void exitKernel(void* processStackPtr){

    // save kernel registers on kernel stack
    asm("stmfd sp!, {r0-r12, lr}");


    // //register level memory dump
    // void* psp = processStackPtr;
    // bwprintf(COM2, "Stackptr: \t %x\r\n", psp);


    // int i;
    // int temp;
    // for(i=0;i<17;i++){
    //     asm("ldr %0, [%1]" :"=r"(temp):"r"(psp));
    //     bwprintf(COM2, "R%d \t %x at %d\r\n", 16-i, temp, psp);
    //     psp += 4;
    // }


    //change to user mode
    // cpsr <- *processStackPtr

    asm("ldr r1, [r0]");
    asm("msr cpsr_c, r1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("add sp, r0, #4");

    //loads user mode registers
    //includes the PC register, and starts executing
    asm("ldmfd SP!, {r0-pc}");
}

__attribute__((naked)) void enterKernel(){
    asm("LDMFD SP!, {R0-R12, LR}");
    asm("MOV PC, LR");
}
