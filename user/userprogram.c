#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>

void userCall(){

    bwprintf(COM2, "%s" ,"Function entry\r\n");

    asm("mov R5, #1488");

        
    asm("MOV R0, #1");
    asm("MOV R1, PC");
    asm("BL bwputr(PLT)");

    int i = GetTid();

    bwprintf(COM2, "%s", "Look at that, I survived bitch\r\n");

}