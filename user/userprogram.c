#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>
#include <stdlib.h>
#include <map.h>

void k1_child() {
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Yield();
    bwprintf(COM2, "%d %d\r\n", MyTid(), MyParentTid());
    Exit();
}

void k1_main(){
    int ret = Create(-1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(-1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    ret = Create(1, k1_child);
    bwprintf(COM2, "Created: %d\r\n", ret);
    bwprintf(COM2, "FirstUserTask: Exit\r\n");
}

void user_main() {
    int who;
    char buf[100];
    Receive(&who, buf, 40);
    Yield();
}

void magicExit(){
    asm("MOV R1, LR");
    asm("MOV R0, #1");
    asm("BL bwputr");
    int i = MyTid();

    bwprintf(COM2, "Task ID %d\r\n", i);
}

void MapTest(){
    Map map;
    Map* m = &map;
    initializeMap(m);
    insertMap(m, 1, NULL);
    printTree(m);
    insertMap(m, 2, NULL);
    printTree(m);
    insertMap(m, 3, NULL);
    printTree(m);
    insertMap(m, 4, NULL);
    printTree(m);
    insertMap(m, 5, NULL);
    printTree(m);
    insertMap(m, 6, NULL);
    printTree(m);
    insertMap(m, 7, NULL);
    printTree(m);
    insertMap(m, 8, NULL);
    printTree(m);
    insertMap(m, 9, NULL);
    printTree(m);
    removeMap(m, 23);
    printTree(m);
    removeMap(m, 25);
    insertMap(m, 35, NULL);
    insertMap(m, 6, NULL);
    insertMap(m, 2, NULL);
    insertMap(m, 5, NULL);
    insertMap(m, 43, NULL);
    insertMap(m, 62, NULL);
    removeMap(m, 63);
    removeMap(m, 35);
    printTree(m);
    insertMap(m, 15, NULL);
}