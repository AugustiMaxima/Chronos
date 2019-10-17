#include <idle.h>
void idle(){
    int i;
    for(;;){
        i = *(volatile int*)0x80930008;
    }
}