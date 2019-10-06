#ifndef PRIORITY_QUEUE
#define PRIORITY_QUEUE

#include <task.h>

/**
 * Fixed size, efficient, runs great
 **/

//Has to be pure exponents of 2 - 1
#define MINHEAP_SIZE 127

typedef struct KVPair{
    int key;
    void* value;
} KV;

typedef struct minHeap{
    int length;
    KV heap[MINHEAP_SIZE];
} MinHeap;

KV* peek(MinHeap* heap);

void initializeMinHeap(MinHeap* heap);

void* removeMinHeap(MinHeap* heap);

int insertMinHeap(MinHeap* heap, int key, void* value);

#endif
