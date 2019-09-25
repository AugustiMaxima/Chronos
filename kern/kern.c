#include <kern.h>
#include <bwio.h>
#include <dump.h>

__attribute__((naked)) void exitKernel(void* processStackPtr){

    // save kernel registers on kernel stack
    asm("STMFD SP!, {R0-PC}");
    asm("ADD R2, PC, #24");
    //sets where the return address should be
    asm("STR R2, [SP, #60]");

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

    asm("LDR R1, [R0]");
    asm("MSR CPSR_c, R1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("ADD SP, R0, #4");

    //loads user mode registers
    //includes the PC register, and starts executing
    asm("LDMFD SP!, {R0-PC}");

    asm("MOV PC, LR");
}

__attribute__((naked)) void enterKernel(){
    asm("LDMFD SP!, {R0-PC}");
}
