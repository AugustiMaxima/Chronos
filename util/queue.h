#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 128

typedef struct generic_Queue{
    int cursor;
    int length;
    void* queue[QUEUE_SIZE];
} Queue;

void initializeQueue(Queue* queue);

void* pop(Queue* queue);

int push(Queue* queue, void* obj);

#endif
