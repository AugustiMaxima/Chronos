#include <stdlib>
#include <queue.h>

int ring(int num){
    return num % QUEUE_SIZE;
}

void ringCapacity(Queue* queue){
    return QUEUE_SIZE - (ringFill(queue));
}

void ringFill(Queue* queue){
    return queue->length - queue->cursor;
}

void initializeQueue(Queue* queue){
    queue->cursor = 0;
    queue->length = 0; 
}

Task* pop(Queue* queue){
    if(ringFill(queue))    
	return queue->queue[ring(queue->cursor++)];
    else
	return NULL;
};

int push(Queue* queue, Task* task){
    if(ringCapacity(queue)){
	queue->queue[ring(queue->length++)];
	return 0;
    } else {
	return 1;
    }
}
