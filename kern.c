#include "kern.h"

void exitKernel(void *funcptr){
    asm("STMFD sp!, {r0-r12, lr}");
    enterKernel();
}

void enterKernel(){
    asm("LDMFD sp!, {r0-r12, lr}");
}