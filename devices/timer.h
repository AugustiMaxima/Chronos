#ifndef TIMER_H
#define TIMER_H

/*
* some timer related stuff, hopefully useful for future endeavors.
*/

void initializeTimer(int timer, int frequency, unsigned int length, int mode);

unsigned int getValue(int timer);

unsigned int getWrap(int timer);

int* getTimerBase(int timer);

#endif
