#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>

void testTask() {
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Exit();
}

void userCall(){

    int i;

    int ret = Create(testTask, -1);
    bwprintf(COM2, "Created: %d\r\n", ret);

    ret = Create(testTask, -1);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(testTask, 1);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(testTask, 1);
    bwprintf(COM2, "Created: %d\r\n", ret);
    bwprintf(COM2, "FirstUserTask: Exit\r\n");

    Exit();
}

void magicExit(){
    int i = MyTid();

    bwprintf(COM2, "Register %d\r\n", i);
}