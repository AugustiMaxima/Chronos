#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

#include <task.h>

/**
 * Fixed size, efficient, runs great
 **/

//Has to be pure exponents of 2 - 1
#define QUEUE_SIZE 127

typedef struct osPQueue{
    int length;
    Task* queue[QUEUE_SIZE];

} PriorityQueue;


Task* removeMin(PriorityQueue* queue);

int insert(PriorityQueue* queue, Task* payload);

#endif