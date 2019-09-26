#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>

void testTask() {
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Exit();
}

void userCall(){

    int i;
    for (i=0; i<4; i++) {
        int ret = Create(userCall, 1);
        bwprintf(COM2, "Created: %d\r\n", ret);
    }
    bwprintf(COM2, "FirstUserTask: Exit\r\n");

    Exit();
}
