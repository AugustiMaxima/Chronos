#include <stdlib.h>
#include <interrupt.h>
#include <deviceRegistry.h>
#include <bwio.h>

extern Scheduler* scheduler;

void initializeDeviceRegistry(DeviceRegistry* registry) {
    initializeQueue(&(registry->freeQueue));
    initializeQueue(&(registry->waitQueue));
    initializeMap(&(registry->deviceMap));
    initializeMap(&(registry->taskWaitMap));
    int i;
    for(i=0;i<MAX_TASKS;i++){
        push(&(registry->freeQueue), registry->waitPool + i);
    }
    for(i=0;i<MAX_MULTI_AWAIT;i++){
	push(&(registry->waitQueue), registry->multiWaitPool + i);
    }
}

int createDeviceWaitEntry(DeviceRegistry* registry, Task* task, int device, TaskNode* node){
    node->task = task;
    node->next = getMap(&(registry->deviceMap), device);
    if(node->next){
	    node->next->prev = node;
	    bwprintf(COM2, "Warning: Multiple tasks awaits the same device signal at device %d\r\n", device);
    }
    node->prev = NULL;
    return putMap(&(registry->deviceMap), device, node);
}

int WaitForDevice(DeviceRegistry* registry, Task* task, int device){
    TaskNode* node = pop(&(registry->freeQueue));
    // bwprintf(COM2, "Retrieved node %x for task %x & device %d\r\n", node, task, device);
    if(!node){
        return -1;
    }
    return createDeviceWaitEntry(registry, task, device, node);
}

int removeNullEntries(DeviceRegistry* registry, TaskAwait* wait, int device, int value){
    if(!wait)
	return 0;
    *(wait->returnArg) = value;
    for(int i=0; i<wait->count; i++){
    	TaskNode* node = wait->registry[i];
	if(wait->key[i] != device){
	    //A thorough clean up and removal of the entry, as if it never was registered
	    if(node->prev){
		    node->prev->next = node->next;
	    } else {
		    putMap(&(registry->deviceMap), wait->key[i], node->next);
	    }
	    if(node->next){
		    node->next->prev = node->prev;
	    }
	    push(&(registry->freeQueue), node);
	} else {
	    node->task->stackEntry[1] = device;
	}
    }
    push(&(registry->waitQueue), wait);
    return 1; 
}

int WakeForDevice(DeviceRegistry* registry, int device, int value){
    int status = 0;
    TaskNode* node = removeMap(&(registry->deviceMap), device);
    // bwprintf(COM2, "Retrieved node %x for device %d\r\n", node, device);
    while(node){
	int multiple = removeNullEntries(registry, getMap(&(registry->taskWaitMap), node->task->tId), device, value);
        if(!multiple){
	        node->task->stackEntry[1] = value;
	    }
        status = insertTaskToQueue(scheduler, node->task);
        push(&(registry->freeQueue), node);
        node = node->next;
    }
    return status;
}

int WaitMultipleDevice(DeviceRegistry* registry, Task* task, int* retainer, int deviceCount, int devices[]){
    //All of nothing resource claim
    if(ringFill(&(registry->freeQueue)) < deviceCount)
	return -1;
    TaskAwait* wait = pop(&(registry->waitQueue));
    if(!wait){
	    return -1;
    }
    wait->count = deviceCount;
    wait->returnArg = retainer;
    for(int i=0;i<deviceCount;i++){
    	wait->registry[i] = pop(&(registry->freeQueue));
	    createDeviceWaitEntry(registry, task, devices[i], wait->registry[i]);
	    wait->key[i] = devices[i];
    }
    insertMap(&(registry->taskWaitMap), task->tId, wait);
    return 0;
}
