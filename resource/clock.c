#include <clock.h>

void initializeClock(Clock* clock, int timer, int frequency, int hours, int minutes, int seconds, int miliseconds){
    clock->timer = timer;
    clock->frequency = frequency;
    initializeTimer(timer, frequency, getWrap(timer), 0);
    initializeTimeStamp(&(clock->time), hours, minutes, seconds, miliseconds);
    clock->lastRead = getWrap(timer);
    clock->underflow = 0;
}

void updateTime(Clock* clock, int delta){
    clock->underflow += delta;
    if(clock->underflow >= clock->frequency/1000){
        applyDeltaTime(&(clock->time), clock->underflow/(clock->frequency/1000));
        clock->underflow %= (clock->frequency/1000);
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

void getCurrentTime(Clock* clock, TimeStamp* time){
    timeElapsed(clock);
    initializeTimeStamp(time, clock->time.hours, clock->time.minutes, clock->time.seconds, clock->time.miliseconds);
}

void initializeTimeStamp(TimeStamp* time, int hours, int minutes, int seconds, int miliseconds){
    time->hours = hours;
    time->minutes = minutes;
    time->seconds = seconds;
    time->miliseconds = miliseconds;
}

void applyDeltaTime(TimeStamp* time, int ms){
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
    int delta = 0;
    delta += a->hours - b->hours;
    delta *= 60;
    delta += a->minutes - b->minutes;
    delta *= 60;
    delta += a->seconds - b->seconds;
    delta *= 1000;
    delta += a->miliseconds - b->miliseconds;
    return delta;
}
