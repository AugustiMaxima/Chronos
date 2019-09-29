#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <task.h>
#include <queue.h>
#include <map.h>
#include <priorityQueue.h>

#define MAX_TASKS 24

typedef struct os_Scheduler{
    Task tasks[MAX_TASKS];
    Queue freeQueue;
    Task* currentTask;
    Map taskTable;
    PriorityQueue readyQueue;
} Scheduler;

//TODO: This probably should be in some debugging file
void printRegisters(int* stack);

void initializeScheduler(Scheduler* scheduler);

int scheduleTask(Scheduler* scheduler, int priority, int parent, void* functionPtr);

void freeTask(Scheduler* scheduler, Task* task);

int runFirstAvailableTask(Scheduler* scheduler);

void runTask(Scheduler* scheduler, Task* task);

int insertTaskToQueue(Scheduler* scheduler, Task* task);

Task* getTask(Scheduler* scheduler, int tId);

void handleSuspendedTasks(void* lr);

#endif
