#include <stdlib.h>
#include <syslib.h>
#include <timer.h>
#include <nameServer.h>
#include <minHeap.h>
#include <queue.h>
#include <charay.h>
#include <clockServer.h>
#include <bwio.h>

#define MAX_ASYNC_POOL 32

void clockNotifier() {
    int tId = 0;

    clockRequest request;
    request.method = TICK;

    Receive(&tId, NULL, 0);
    Reply(tId, NULL, 0);

    for (;;) {
        AwaitEvent(TC1UI_DEV_ID);
        Send(tId, (const char*)&request, sizeof(request), NULL, 0);
    }
}

void clockServer() {    

    int ticks = 0;
    MinHeap waitList;
    MinHeap workList;

    initializeMinHeap(&waitList);
    initializeMinHeap(&workList);

    AsyncWork asyncRequests[MAX_ASYNC_POOL];
    Queue asyncResourceQueue;

    initializeQueue(&asyncResourceQueue);
    for(int i=0;i<MAX_ASYNC_POOL;i++){
        push(&asyncResourceQueue, asyncRequests + i);
    }

    clockRequest request;
    int caller;
    int reply;

    RegisterAs("cs");

    //For the sake of standardization, all events waiting for device interrupts gets -2
    //If one event is particular sensitive, bump it up to -3
    int notifier = Create(-3, clockNotifier);
    Send(notifier, NULL, 0, NULL, 0);

    while(1) {
        Receive(&caller, (char*)&request, sizeof(request));
        if (request.method == TICK) {
            ticks++;
            Reply(caller, NULL, 0);

            KV* taskWait = peek(&waitList);

            while(taskWait && taskWait->key <= ticks){
                //A little dangerous, copies a literal into the char array and attempt to dereference it on the other end
                reply = ticks;
                Reply((int)(taskWait->value), (char*)&reply, sizeof(reply));
                removeMinHeap(&waitList);
                taskWait = peek(&waitList);
            }

            taskWait = peek(&workList);

            while(taskWait && taskWait->key <= ticks){
                AsyncWork* work = taskWait->value;
                Reply(work->tId, work->workDescriptor, work->length);
                removeMinHeap(&workList);
                push(&asyncResourceQueue, work);
                taskWait = peek(&workList);
            }

        } else if (request.method == TIME) {
            reply = ticks;
            Reply(caller, (char*)&reply,  sizeof(reply));
        } else {            
            int requestTicks = request.tick;
            if(request.method == DELAY || request.method == REPLY){
                requestTicks += ticks;
            }
            if(requestTicks<ticks){
                reply = -2;
                Reply(caller, (char*)&reply, sizeof(reply));
            }
            else {
                if(request.method == DELAY || request.method == DELAY_UNTIL){
                    insertMinHeap(&waitList, requestTicks, (void*)caller);
                } else {
                    //acks and stores the details
                    AsyncWork* work = pop(&asyncResourceQueue);
                    if(!work){
                        reply = -1;
                    } else {
                        insertMinHeap(&workList, requestTicks, (void*)work);
                        work->tId = request.payload->tId;
                        work->length = request.payload->length > 32? 32 : request.payload->length;
                        for(int i=0; i<32 && i<request.payload->length; i++){
                            work->workDescriptor[i] = request.payload->payload[i];
                        }
                    }
                    Reply(caller, (char*)&reply, sizeof(reply));
                }
            }
        }
    }
}

int DelayUntil(int csTid, int deadline){
    clockRequest request;
    request.method = DELAY_UNTIL;
    request.tick = deadline;
    int result;
    int status = Send(csTid, (const char*)&request, sizeof(request), (char*)&result, sizeof(result));
    if(status==-1)
        return status;
    return result;
}

int Time(int csTid){
    clockRequest request;
    request.method = TIME;
    int result;
    int status = Send(csTid, (const char*)&request, sizeof(request), (char*)&result, sizeof(result));
    if(status==-1)
        return status;
    return result;
}

int Delay(int csTid, int ticks) {
    clockRequest request;
    request.method = DELAY;
    request.tick = ticks;
    int result;
    int status = Send(csTid, (const char*)&request, sizeof(request), (char*)&result, sizeof(result));
    if(status==-1)
        return status;
    return result;
}

int AsyncDelay(int csTid, int ticks, int tId, int length, void* payload){
    clockWork work;
    work.length = length;
    work.tId = tId;
    work.payload = payload;
    clockRequest request;
    request.method = REPLY;
    request.tick = ticks;
    request.payload = &work;
    int result;
    int status = Send(csTid, (const char*)&request, sizeof(request), (char*)&result, sizeof(result));
    if(status==-1)
        return status;
    return result;
}

int AsyncDelayUntil(int csTid, int deadline, int tId, int length, void* payload){
    clockWork work;
    work.length = length;
    work.tId = tId;
    work.payload = payload;
    clockRequest request;
    request.method = REPLY_UNTIL;
    request.tick = deadline;
    request.payload = &work;
    int result;
    int status = Send(csTid, (const char*)&request, sizeof(request), (char*)&result, sizeof(result));
    if(status==-1)
        return status;
    return result;
}
