#ifndef TASK_H
#define TASK_H

#include <state.h>

#define MAX_CHILDREN 8
#define STACK_SIZE 65536

typedef enum{
    RUNNING,
    READY,
    BLOCKED
} taskStatus;

typedef struct os_Task{
    int pId;
    int tId;
    int priority;
    int childTasks[MAX_CHILDREN];
    char STACK[STACK_SIZE];
    taskStatus status;
} Task;

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status);

int appendChildTasks(Task* task, int tId);

#endif
