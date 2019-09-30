#include <timer.h>

typedef struct {
    int hours;
    int minutes;
    int seconds;
    int miliseconds;
} TimeStamp;


typedef struct osClock{
    int timer;
    int precision;
    TimeStamp time;
    unsigned underflow;
    unsigned lastRead;
} Clock;

void initializeclock(Clock* clock, int timer, int precision, int hours, int minutes, int seconds, int miliseconds);

void updateTime(Clock* clock, int delta);

int timeElapsed(Clock* clock);

// expects a 9 character long buffer
void getTimestring(char* buffer, TimeStamp* time);

void initializeTimeStamp(TimeStamp* time, int hours, int minutes, int seconds, int miliseconds);

void applyDetalTime(TimeStamp* time, int ms);

int compareTime(TimeStamp* a, TimeStamp* b);