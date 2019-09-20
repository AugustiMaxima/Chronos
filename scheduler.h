#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include <task.h>
#include <queue.h>

#define MAX_TASKS 24;

struct os_Scheduler{
    Task tasks[MAX_TASKS];
    Queue readyQueue;
} Scheduler;

void intializeScheduler(Scheduler* scheduler);

int createTask(Scheduler* scheduler);

void freeTask(Scheduler* scheduler, int tId);

#endif
