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

Task* pop(Queue* queue){
    if (ringFill(queue)) {
        Task* ret = queue->queue[ring(queue->cursor++)];
        bwprintf(COM2, "returning %d\r\n", ret);
	    return ret;
    } else {
	    return 0;
    }
};

int push(Queue* queue, Task* task){
    if(ringCapacity(queue)){
	    queue->queue[ring(queue->length++)] = task;
	    return 0;
    } else {
	    return 1;
    }
}
