#include "kern.h"

void exitKernel(void *funcptr){
    asm (" ");
    enterKernel();
}