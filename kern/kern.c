#include <kern.h>
#include <bwio.h>
#include <dump.h>

void exitKernel(void* processStackPtr){

    // save kernel registers on kernel stack
    asm("SUB SP, SP, #64");
    asm("STR R0, [SP]");
    asm("STR R1, [SP, #4]");
    asm("STR R2, [SP, #8]");
    asm("STR R3, [SP, #12]");
    asm("STR R4, [SP, #16]");
    asm("STR R5, [SP, #20]");
    asm("STR R6, [SP, #24]");
    asm("STR R7, [SP, #28]");
    asm("STR R8, [SP, #32]");
    asm("STR R9, [SP, #36]");
    asm("STR R10, [SP, #40]");
    asm("STR R11, [SP, #44]");
    asm("STR R12, [SP, #48]");
    asm("STR R13, [SP, #52]");
    asm("STR R14, [SP, #56]");
    asm("ADD R2, PC, #80");
    //sets where the return address should be
    asm("STR R2, [SP, #60]");

    asm("ldr r1, [r0]");
    asm("msr cpsr_c, r1");

    //restores the stack pointer, minus the cpsr
    // sp <- r0 - 4
    asm("add sp, r0, #4");

    //loads user mode registers
    //includes the PC register, and starts executing

    asm("LDR R0, [SP]");
    asm("LDR R1, [SP, #4]");
    asm("LDR R2, [SP, #8]");
    asm("LDR R3, [SP, #12]");
    asm("LDR R4, [SP, #16]");
    asm("LDR R5, [SP, #20]");
    asm("LDR R6, [SP, #24]");
    asm("LDR R7, [SP, #28]");
    asm("LDR R8, [SP, #32]");
    asm("LDR R9, [SP, #36]");
    asm("LDR R10, [SP, #40]");
    asm("LDR R11, [SP, #44]");
    asm("LDR R12, [SP, #48]");
    asm("LDR R13, [SP, #52]");
    asm("LDR R14, [SP, #56]");
    asm("ADD SP, SP, #64");
    asm("LDR R15, [SP, #-4]");

    bwprintf(COM2, "end of exitKernel\r\n");
}

void __attribute__((naked)) enterKernel(){
    bwprintf(COM2, "entering Kernel\r\n");
    asm("LDR R0, [SP]");
    asm("LDR R1, [SP, #4]");
    asm("LDR R2, [SP, #8]");
    asm("LDR R3, [SP, #12]");
    asm("LDR R4, [SP, #16]");
    asm("LDR R5, [SP, #20]");
    asm("LDR R6, [SP, #24]");
    asm("LDR R7, [SP, #28]");
    asm("LDR R8, [SP, #32]");
    asm("LDR R9, [SP, #36]");
    asm("LDR R10, [SP, #40]");
    asm("LDR R11, [SP, #44]");
    asm("LDR R12, [SP, #48]");
    asm("LDR R13, [SP, #52]");
    asm("LDR R14, [SP, #56]");
    asm("ADD SP, SP, #64");
    asm("LDR R15, [SP, #-4]");
}
