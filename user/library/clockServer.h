#ifndef CLOCKSERVER
#define CLOCKSERVER

typedef struct async_Work{
    int tId;
    int length;
    char workDescriptor[32];
} AsyncWork;

typedef struct clockAsyncRequest{
    int length;
    int tId;
    char* payload;
} clockWork;

typedef struct clockServerRequest{
    enum clockRequestMethod{
        TICK,
        TIME,
        DELAY,
        DELAY_UNTIL,
        REPLY,
        REPLY_UNTIL
    } method;
    int tick;
    clockWork* payload;
} clockRequest;

void clockServer();

int DelayUntil(int csTid, int deadline);

int Time(int csTid);

int Delay(int csTid, int ticks);

int AsyncDelay(int csTid, int ticks, int tId, int length, void* payload);

int AsyncDelayUntil(int csTid, int deadline, int tId, int length, void* payload);

#endif