#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_SIZE 24

typedef struct int_Queue{
    int cursor;
    int length;
    int queue[QUEUE_SIZE];
} Queue;

void initializeQueue(Queue* queue);

int peep(Queue* queue);

int pop(Queue* queue);

int push(Queue* queue, int obj);

#endif
