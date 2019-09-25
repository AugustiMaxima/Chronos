#include <syslib.h>

int createTask(void* functionPtr, int priority){
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
    asm("ADD R2, PC, #32");
    //sets where the return address should be
    asm("STR R2, [SP, #60]");
    asm("SUB SP, SP, #4");
    asm("MRS R2, CPSR");
    asm("STR R2, [SP]");
    //argument stuffing now
    asm("SUB SP, SP, #8");
    asm("STR R0, [SP]");
    asm("STR R1, [SP, #4]");
    asm("SWI 1");
    //<---- PC return points here
}

int getPId(){

}

int getTId(){
    int tId = 0;

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
    asm("ADD R2, PC, #32");
    //sets where the return address should be
    asm("STR R2, [SP, #60]");
    asm("SUB SP, SP, #4");
    asm("MRS R2, CPSR");
    asm("STR R2, [SP]");
    //argument stuffing now
    asm("SWI 0");
    asm("mov %0, R0" :"=r"(tId));
    return tId;
}