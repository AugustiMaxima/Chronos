#include <clock.h>
#define MAX4294967295

void initializeclock(Clock* clock, int timer, int precision, int hours, int minutes, int seconds, int miliseconds){
    clock->timer = timer;
    clock->precision = precision;
    initializeTimer(timer, precision, getWrap(timer), 0);
    initializeTimeStamp(&(clock->time), hours, minutes, seconds, miliseconds);
    clock->lastRead = getWrap(timer);
    clock->underflow = 0;
}

void updateTime(Clock* clock, int delta){
    clock->underflow += delta;
    if(clock->underflow >= clock->precision/1000){
        applyDeltaTime(&(clock->time), clock->underflow/(clock->precision/1000));
        clock->underflow %= (clock->precision/1000);
    }
}

int timeElapsed(Clock* clock){
    int elapsed;
    unsigned int newValue = getValue(clock->timer);
    if(newValue > clock->lastRead) {
        elapsed = getWrap(clock->timer) - newValue + clock->lastRead;
    } else {
        elapsed = clock->lastRead - newValue;
    }
    clock->lastRead = newValue;
    updateTime(clock, elapsed);
    return elapsed;

}

// expects a 10 - 12 character long buffer
void getTimeString(char* buffer, TimeStamp* time){
    int hour = time->hours%24;
    int timeSeries[3] = {hour, time->minutes, time->seconds};
    int i;
    for(i=0;i<3;i++){
        buffer[3*i] = '0' + timeSeries[i]/10;
        buffer[3*i+1] = '0' + timeSeries[i]%10;
        buffer[3*i+2] = i==2? '.' : ':';    
    }
    buffer[9] = '0' + time->miliseconds/100;
    buffer[10] = 0;
}

void initializeTimeStamp(TimeStamp* time, int hours, int minutes, int seconds, int miliseconds){
    time->hours = hours;
    time->minutes = minutes;
    time->seconds = seconds;
    time->miliseconds = miliseconds;
}

void applyDetalTime(TimeStamp* time, int ms){
    time->miliseconds += ms;
    if(time->miliseconds >= 1000) {
        time->seconds += time->miliseconds/1000;
        time->miliseconds %= 1000;
    }
    if(time->seconds >= 60) {
        time->minutes += time->seconds/60;
        time->seconds %= 60;
    }
    if(time->minutes >= 60) {
        time->hours += time->minutes/60;
        time->minutes %= 60;
    }
}

int compareTime(TimeStamp* a, TimeStamp* b){
    if(a->hours == b->hours){
        if(a->minutes == b->minutes){
            if(a->seconds == b->seconds){
                return a->miliseconds - b->miliseconds;
            }
            return a->seconds - b->seconds;
        }
        return a->minutes - b->minutes;
    }
    return a->hours - b->hours;
}
