#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>
#include <stdlib.h>
#include <map.h>

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
    return *s1 - *s2;
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
char* WON_MSG = "won";
char* LOST_MSG = "lost";
char* DRAW_MSG = "draw";
char* ROCK_MSG = "rock";
char* I_QUIT_MSG = "i_quit";
char* THEY_QUIT_MSG = "they_quit";

int isMoveStr(const char* msg) {
    if (0 == strcmp(msg, ROCK_MSG)) return 1;
    return 0;
}

int isMoveChar(char msg) {
    if (msg == 'r') return 1;
    return 0;
}

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
    char p1Move = 'x';
    char p2Move = 'x';

    initializeQueue(&signups);

    while (1) {
        trace_Receive("rpsServer", &who, buf);

        // handle receive
        if (0 == strcmp(buf, SIGNUP_MSG)) {
            push(&signups, who);
        } else if (p1 == who && isMoveStr(buf)) {
            p1Move = buf[0];
        } else if (p2 == who && isMoveStr(buf)) {
            p2Move = buf[0];
        } else if (p1 == who && 0 == strcmp(buf, I_QUIT_MSG)) {
            p1Move = 'q';
        } else if (p2 == who && 0 == strcmp(buf, I_QUIT_MSG)) {
            p2Move = 'q';
        } else {
            bwprintf(COM2, "[rpsServer]\tUnknown message\r\n");
        }

        if (p1 == -1 && 2 == ringFill(&signups)) {
            p1 = pop(&signups);
            p2 = pop(&signups);
            Reply(p1, FIRSTCHOICE_MSG, strlen(FIRSTCHOICE_MSG));
            Reply(p2, FIRSTCHOICE_MSG, strlen(FIRSTCHOICE_MSG));
        } else if (isMoveChar(p1Move) && isMoveChar(p2Move)) {
            int p1Won = 1; // todo: check who won
            p1Move = 'x';
            p2Move = 'x';
            Reply(p1, DRAW_MSG, strlen(DRAW_MSG));
            Reply(p2, DRAW_MSG, strlen(DRAW_MSG));
        } else if (p1Move == 'q') {
            Reply(p2, THEY_QUIT_MSG, strlen(THEY_QUIT_MSG));
            p1 = -1;
            p2 = -1;
            p1Move = 'x';
            p2Move = 'x';
        } else if (p2Move == 'q') {
            Reply(p1, THEY_QUIT_MSG, strlen(THEY_QUIT_MSG));
            p1 = -1;
            p2 = -1;
            p1Move = 'x';
            p2Move = 'x';
        }
    }
}

void rpsClient() {
    char buf[100];
    trace_Send("rpsClient", 2, SIGNUP_MSG, buf);
    if (0 == strcmp(buf, FIRSTCHOICE_MSG)) {
        trace_Send("rpsClient", 2, ROCK_MSG, buf);
        if (0 == strcmp(buf, THEY_QUIT_MSG)) Exit();
        trace_Send("rpsClient", 2, I_QUIT_MSG, buf);
    }
}

void rpsClient2() {
    char buf[100];
    trace_Send("rpsClient", 2, SIGNUP_MSG, buf);
    if (0 == strcmp(buf, FIRSTCHOICE_MSG)) {
        trace_Send("rpsClient", 2, ROCK_MSG, buf);
        if (0 == strcmp(buf, THEY_QUIT_MSG)) Exit();
        trace_Send("rpsClient", 2, ROCK_MSG, buf);
        if (0 == strcmp(buf, THEY_QUIT_MSG)) Exit();
        trace_Send("rpsClient", 2, I_QUIT_MSG, buf);
    }
}

void k2_main() {
    Create(-1, rpsServer);
    Create(5, rpsClient);
    Create(5, rpsClient2);
    Create(6, rpsClient);
    Create(6, rpsClient2);
}

