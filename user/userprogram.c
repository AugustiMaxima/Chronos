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

void shitTest(){
    Destroy();
}

void FireStrike(){
    int i=0;
    int exp = 0;
    for(i = 0; i<10000; i++){
        int id = Create(2, shitTest);
        if(id>0){
            exp++;
        }
    }
    bwprintf(COM2, "Created %d tasks\r\n", exp);
}

// https://stackoverflow.com/questions/34873209/implementation-of-strcmp
int strcmp(char* s1, char* s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// gcc is generating memcpys
// https://code.woboq.org/gcc/libgcc/memcpy.c.html
void * memcpy (void *dest, const void *src, size_t len) {
  char *d = dest;
  const char *s = src;
  while (len--)
    *d++ = *s++;
  return dest;
}

// https://code.woboq.org/userspace/glibc/string/test-strlen.c.htmlsize_t
strlen (const char *s) {
  const char *p;
  for (p = s; *p; ++p);
  return p - s;
}

char* SIGNUP_MSG = "signup";
char* FIRSTCHOICE_MSG = "first";

void trace_Send(const char* taskName, int tid, const char* msg, char* reply) {
    int ret = Send(tid, msg, strlen(msg), reply, 100);
    bwprintf(COM2, "[%s %d]\t%d = Send(%d, %s, =%s)\r\n", taskName, MyTid(), ret, tid, msg, reply);
}

void trace_Receive(const char* taskName, int* tid, char* msg) {
    int ret = Receive(tid, msg, 100);
    bwprintf(COM2, "[%s %d]\t%d = Receive(=%d, =%s)\r\n", taskName, MyTid(), ret, *tid, msg);

}

void rpsServer() {
    // Queue<int>
    Queue signups;
    char buf[100];
    int who;
    int p1 = -1;
    int p2 = -1;

    initializeQueue(&signups);

    while (1) {
        trace_Receive("rpsServer", &who, buf);

        // handle receive
        if (0 == strcmp(buf, SIGNUP_MSG)) {
            push(&signups, who);
        } else {
            bwprintf(COM2, "[rpsServer]\tUnknown message\r\n");
        }

        if (p1 == -1 && 2 == ringFill(&signups)) {
            p1 = pop(&signups);
            p2 = pop(&signups);
            Reply(p1, FIRSTCHOICE_MSG, strlen(FIRSTCHOICE_MSG));
            Reply(p2, FIRSTCHOICE_MSG, strlen(FIRSTCHOICE_MSG));
        }
    }
}

void rpsClient() {
    char buf[100];
    trace_Send("rpsClient", 2, SIGNUP_MSG, buf);
}

void k2_main() {
    Create(-1, rpsServer);
    Create(5, rpsClient);
    Create(5, rpsClient);
}

