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

void receiver() {
    int who;
    char buf[100];
    int ret = Receive(&who, buf, 40);
    bwprintf(COM2, "[receiver]\tReceive(=%d, =%s, 40)=%d\r\n", who, buf, ret);
    Reply(who, "Who the fuck are ye?", 40);
}

void sender() {
    char buf[100];
    int ret = Send(2, "hello", 40, buf, 40);
    bwprintf(COM2, "[sender]\tSend(2, hello, 40, =%s, 40)=%d\r\n", buf, ret);
}

void user_main() {
    bwprintf(COM2, "[user_main]\tCreate(-1, receiver)=%d\r\n", Create(-1, receiver));
    bwprintf(COM2, "[user_main]\tCreate(-1, sender)=%d\r\n", Create(-1, sender));
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
