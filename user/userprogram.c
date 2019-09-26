#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>

void userCall(){

    bwprintf(COM2, "%s" ,"Function entry\r\n");

    Exit();
}