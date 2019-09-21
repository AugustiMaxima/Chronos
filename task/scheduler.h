#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <task.h>
#include <queue.h>

#define MAX_TASKS 24

typedef struct os_Scheduler{
    Task tasks[MAX_TASKS];
    Task* currentTask;
    Queue readyQueue;
} Scheduler;

void intializeScheduler(Scheduler* scheduler);

int createTask(Scheduler* scheduler);

void freeTask(Scheduler* scheduler, int tId);

void* runTask(Scheduler* scheduler, int tId);

#endif
