#ifndef CLOCKSERVER_H
#define CLOCKSERVER_H

void clockServer();

int DelayUntil(int csTid, int deadline);

int Time(int csTid);

int Delay(int csTid, int ticks);

void notifier();

#endif