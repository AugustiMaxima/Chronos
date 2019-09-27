#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>

void testTask() {
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Yield();
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Exit();
}

void userCall(){
    int ret = Create(-1, testTask);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(-1, testTask);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(1, testTask);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(1, testTask);
    bwprintf(COM2, "Created: %d\r\n", ret);
    bwprintf(COM2, "FirstUserTask: Exit\r\n");

}

void magicExit(){
    asm("MOV R1, LR");
    asm("MOV R0, #1");
    asm("BL bwputr");
    int i = MyTid();

    bwprintf(COM2, "Task ID %d\r\n", i);
}