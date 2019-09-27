#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <task.h>
#include <priorityQueue.h>

#define MAX_TASKS 24

typedef struct os_Scheduler{
    Task tasks[MAX_TASKS];
    Task* currentTask;
    PriorityQueue queue;
} Scheduler;


void initializeScheduler(Scheduler* scheduler);

int scheduleTask(Scheduler* scheduler, int priority, int parent, void* functionPtr);

void freeTask(Scheduler* scheduler, int tId);

int runFirstAvailableTask(Scheduler* scheduler);

void runTask(Scheduler* scheduler, int tId);

int insertTaskToQueue(Scheduler* scheduler, Task* task);

Task* getTask(Scheduler* scheduler, int tId);

void handleSuspendedTasks(void* lr);

#endif
