#ifndef DEVICE_REGISTRY
#define DEVICE_REGISTRY

#include <scheduler.h>
#define MAX_MULTI_AWAIT 16

typedef struct taskNode{
    Task* task;
    struct taskNode* prev;
    struct taskNode* next;
} TaskNode;

typedef struct multiAwait{
    int count;
    int* returnArg;
    TaskNode* registry[5];
    int key[5];
} TaskAwait;

typedef struct DeviceTracker{
    TaskNode waitPool[MAX_TASKS];
    TaskAwait multiWaitPool[MAX_MULTI_AWAIT];
    Queue freeQueue;
    Queue waitQueue;
    Map deviceMap;
    //Used to support multiple await
    Map taskWaitMap;
} DeviceRegistry;

void initializeDeviceRegistry(DeviceRegistry* registry);
int WaitForDevice(DeviceRegistry* registry, Task* task, int device);
int WakeForDevice(DeviceRegistry* registry, int device, int value);
int WaitMultipleDevice(DeviceRegistry* registry, Task* task, int* retainer, int argCount, int devices[]);

#endif
