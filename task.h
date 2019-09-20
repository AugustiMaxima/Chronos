#ifndef TASK_H
#define TASK_H

#include <state.h>

#define MAX_CHILDREN 8
#define STACK_SIZE 

typedef struct os_Task{
    int pId;
    int tId;
    int priority;
    int childTasks[MAX_CHILDREN];
} Task;

void initializeTask(Task* task, int tId, int pId, int priority);

int appendChildTasks(Task* task, int tId);

#endif
