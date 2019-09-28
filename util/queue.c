#include <queue.h>
#include <bwio.h>

int ring(int num){
    return num % QUEUE_SIZE;
}

int ringFill(Queue* queue){
    return queue->length - queue->cursor;
}

int ringCapacity(Queue* queue){
    return QUEUE_SIZE - (ringFill(queue));
}

void initializeQueue(Queue* queue){
    queue->cursor = 0;
    queue->length = 0;
}

void* pop(Queue* queue){
    if (ringFill(queue)) {
        int ret = queue->queue[ring(queue->cursor++)];
	    return ret;
    } else {
	    return 0;
    }
};

int peep(Queue* queue){
    if (ringFill(queue)) {
        int ret = queue->queue[ring(queue->cursor)];
	    return ret;
    } else {
	    return 0;
    }
};

int push(Queue* queue, void* obj){
    if(ringCapacity(queue)){
	    queue->queue[ring(queue->length++)] = obj;
	    return 0;
    } else {
	    return 1;
    }
}
