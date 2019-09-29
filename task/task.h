#ifndef TASK_H
#define TASK_H

#define STACK_SIZE 65536

typedef enum task_Status{
    RUNNING,
    READY,
    BLOCKED,
    EXITED
} taskStatus;

typedef struct os_Task{
    int pId; // parentId
    int tId; // taskId
    int priority;

    char STACK[STACK_SIZE];

    // current sp
    int* stackEntry;

    taskStatus status;
} Task;

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status, void* functionPtr);

#endif
