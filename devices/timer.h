#ifndef TIMER_H
#define TIMER_H

/*
* some timer related stuff, hopefully useful for future endeavors.
*/

#define TC1UI_DEV_ID 4
#define TC2UI_DEV_ID 5

void initializeTimer(int timer, int frequency, unsigned int length, int mode);

void disableTimer();

unsigned int readTimerValue(int timer);

unsigned int getWrap(int timer);

int getTimerBase(int timer);

void clearTimerInterrupt(int timer);

#endif
