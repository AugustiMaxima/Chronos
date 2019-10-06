#include <bwio.h>
#include <syslib.h>
#include <nameServer.h>

void idle() {
    for (;;) {}
}

typedef struct waitRequest_struct {
    int tId;
    int deadline;
} WaitRequest;

#define MAX_WAITING_TASKS 4
#define MAX_REQUEST 100
void clockServer() {

    int ticks = 0;
    WaitRequest wr[MAX_WAITING_TASKS];
    for (int i=0; i<MAX_WAITING_TASKS; i++) {
        wr[i].tId = -1;
    }

    RegisterAs("cs");

    char requestBuf[MAX_REQUEST];
    char command;
    int caller;
    char* symbol;

    while(1) {
        Receive(&caller, requestBuf, 100);
        if (0 == strcmp(requestBuf, "tick")) {
            ticks += 1;
            Reply(caller, "ok", strlen("ok"));

            // process wr
            for (int i=0; i<MAX_WAITING_TASKS; i++) {
                if (wr[i].tId != -1 && wr[i].deadline <= ticks) {
                    Reply(wr[i].tId, "ok", strlen("ok"));
                    wr[i].tId = -1;
                }
            }
        } else if (0 == strcmp(requestBuf, "T")) {
            char reply[50];
            formatStrn(reply, 50, "%d", ticks);
            Reply(caller, reply, chos_strlen(reply));
        } else {
            command = requestBuf[0];
            symbol = requestBuf;
            symbol += 2;

            if(command == 'U') {
                int deadline = stringToNum(symbol, 10);

                // insert into wr
                int inserted = 0;
                for (int i=0; i<MAX_WAITING_TASKS; i++) {
                    if (wr[i].tId == -1) {
                        wr[i].tId = caller;
                        wr[i].deadline = deadline;
                        inserted = 1;
                        break;
                    }
                }
                if (!inserted) {
                    bwprintf(COM2, "panic: too many tasks\r\n");
                    while (1) {}
                }
            } else {
                bwprintf(COM2, "unknown\r\n");
            }
        }
    }
}

int DelayUntil(int csTid, int deadline){
    char buffer[100];
    char receiveBuffer[100];
    formatStrn(buffer, 100, "U %d", deadline);
    Send(csTid, buffer, 100, receiveBuffer, 100);
    return 0;
}

int Time(int csTid){
    char receiveBuffer[100];
    Send(csTid, "T", strlen("T"), receiveBuffer, 100);
    return stringToNum(receiveBuffer, 10);
}

int Delay(int csTid, int ticks) {
    int time = Time(csTid);
    return DelayUntil(csTid, time + ticks);
}

void notifier() {
    bwprintf(COM2, "notifier\r\n");

    int tId = 0;
    while(!tId) tId = WhoIs("cs");

    char buf[100];

    for (;;) {
        AwaitEvent(42);
        Send(tId, "tick", strlen("tick"), buf, 100);
    }
}

void delayer(int delayTime, int numDelays) {
    int csTid = WhoIs("cs");
    for (int i=0; i<numDelays; i++) {
        bwprintf(COM2, "time=%d  \ttId=%d\r\n", Time(csTid), MyTid());
        Delay(csTid, delayTime);
    }
}

void delayer1() {
    delayer(10, 20);
    bwprintf(COM2, "delayer1 done\r\n");
}

void delayer2() {
    delayer(23, 9);
    bwprintf(COM2, "delayer2 done\r\n");
}

void delayer3() {
    delayer(33, 6);
    bwprintf(COM2, "delayer3 done\r\n");
}

void delayer4() {
    delayer(71, 3);
    bwprintf(COM2, "delayer4 done\r\n");
}

void k3_main() {
    Create(10, nameServer);
    Create(9, clockServer);
    Create(8, notifier);
    Create(-3, delayer1);
    Create(-4, delayer2);
    Create(-5, delayer3);
    Create(-6, delayer4);
    Create(-10, idle);
}
