#ifndef DEVICE_REGISTRY
#define DEVICE_REGISTRY

#include <scheduler.h>


typedef struct singleLinkedNode{
    Task* task;
    struct singleLinkedNode* next;
} TaskNode;

typedef struct DeviceTracker{
    TaskNode waitPool[MAX_TASKS];
    Queue freeQueue;
    Map deviceMap;
} DeviceRegistry;

void initializeDeviceRegistry(DeviceRegistry* registry);
int WaitForDevice(DeviceRegistry* registry, Task* task, int device);
int WakeForDevice(DeviceRegistry* registry, int device, int value);

#endif