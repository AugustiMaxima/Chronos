#include <minHeap.h>
#include <clockServer.h>
#include <nameServer.h>
#include <syslib.h>
#include <bwio.h>

#define CLOCK_MAX 24

void clockServer() {
    int ticks = 0;
    MinHeap waitList;

    RegisterAs("cs");
    initializeMinHeap(&waitList);
    char requestBuf[CLOCK_MAX];
    char reply[8];
    char command;
    int caller;
    int requestTicks;

    while(1) {
        Receive(&caller, requestBuf, CLOCK_MAX);
        if (0 == strcmp(requestBuf, "TICK")) {
            ticks++;
            Reply(caller, "ok", strlen("ok"));
            KV* taskWait = peek(&waitList);

            while(taskWait && taskWait->key <= ticks){
                //A little dangerous, copies a literal into the char array and attempt to dereference it on the other end
                *(int*)reply = ticks;
                Reply((int)(taskWait->value), reply, 4);
                removeMinHeap(&waitList);
                taskWait = peek(&waitList);
            }
        } else if (0 == strcmp(requestBuf, "T")) {
            *(int*)reply = ticks;
            Reply(caller, reply,  4);
        } else {
            command = requestBuf[0];
            requestTicks = *(int*)(requestBuf+4);
            if(command == 'D') {
                requestTicks+=ticks;
            } else if (command == 'U'){}
            else {
                bwprintf(COM2, "unknown\r\n");
            }
	    //bwprintf(COM2, "ClockServer: delay %d requested by %d\r\n", requestTicks, caller);
            if(requestTicks<ticks){
                *(int*)reply = -2;
                Reply(caller, reply, 4);
            }
            else {
                insertMinHeap(&waitList, requestTicks, (void*)caller);
            }
        }
    }
}

int DelayUntil(int csTid, int deadline){
    char buffer[12];
    char receiveBuffer[8];
    buffer[0] = 'U';
    *(int*)(buffer + 4) = deadline;
    buffer[9] = 0;
    int status = Send(csTid, buffer, 12, receiveBuffer, CLOCK_MAX);
    if(status==-1)
        return status;
    return *(int*)receiveBuffer;
}

int Time(int csTid){
    char receiveBuffer[8];
    int status = Send(csTid, "T", strlen("T"), receiveBuffer, 8);
    if(status==-1)
        return status;
    return *(int*)receiveBuffer;
}

int Delay(int csTid, int ticks) {
    char buffer[12];
    char receiveBuffer[8];
    buffer[0] = 'D';
    *(int*)(buffer + 4) = ticks;
    buffer[9] = 0;
    int status = Send(csTid, buffer, 12, receiveBuffer, CLOCK_MAX);
    if(status==-1)
        return status;
    return *(int*)receiveBuffer;
}

void notifier() {
    bwprintf(COM2, "notifier\r\n");

    int tId = 0;
    while(!tId) tId = WhoIs("cs");

    char buf[3];

    for (;;) {
        AwaitEvent(51);
        Send(tId, "TICK", strlen("TICK"), buf, 2);
    }
}
