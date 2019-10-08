#include <deviceRegistry.h>
#include <bwio.h>

extern Scheduler* scheduler;

void initializeDeviceRegistry(DeviceRegistry* registry){
    initializeQueue(&(registry->freeQueue));
    initializeMap(&(registry->deviceMap));
    int i;
    for(i=0;i<MAX_TASKS;i++){
        push(&(registry->freeQueue), registry->waitPool + i);
    }
}

int WaitForDevice(DeviceRegistry* registry, Task* task, int device){
    TaskNode* node = pop(&(registry->freeQueue));
    //bwprintf(COM2, "Retrieved node %x for task %x & device %d\r\n", node, task, device);
    if(!node){
        return -1;
    }
    node->task = task;
    node->next = getMap(&(registry->deviceMap), device);
    return putMap(&(registry->deviceMap), device, node);
}

int WakeForDevice(DeviceRegistry* registry, int device, int value){
    int status = 0;
    TaskNode* node = removeMap(&(registry->deviceMap), device);
    //bwprintf(COM2, "Retrieved node %x for device %d\r\n", node, device);
    while(node){
        node->task->stackEntry[1] = value;
        status = insertTaskToQueue(scheduler, node->task);
        push(&(registry->freeQueue), node);
        node = node->next;
    }
    return status;
}
