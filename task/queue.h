#ifndef QUEUE_H
#define QUEUE_H

#include <task.h>

#define QUEUE_SIZE 24

typedef struct task_Queue{
    int cursor;
    int length;
    Task* queue[QUEUE_SIZE];
} Queue;

void initializeQueue(Queue* queue);

Task* pop(Queue* queue){};

int push(Queue* queue, Task* task){};

#endif
