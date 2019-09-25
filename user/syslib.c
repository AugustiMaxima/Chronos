#include <syslib.h>

int createTask(void* functionPtr, int priority){
    asm("STMFD SP!, {R0-PC}");
    asm("MOV R2, PC"); // <--------------- This is the point where the PC is stored, count downwards from here
    asm("ADD R2, #40");
    asm("SUB SP, SP, #4");
    asm("STR R2, [SP, #4]");
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

}   