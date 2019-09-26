#ifndef TASK_H
#define TASK_H

#define MAX_CHILDREN 8
#define STACK_SIZE 65536

typedef enum{
    RUNNING,
    READY,
    BLOCKED,
    EXITED
} taskStatus;

typedef struct os_Task{
    int pId; // parentId
    int tId; // taskId
    int priority;
    int childTasks[MAX_CHILDREN];

    char STACK[STACK_SIZE];

    // current sp
    int* stackEntry;

    taskStatus status;
} Task;

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status, void* functionPtr);

int appendChildTasks(Task* task, int tId);

#endif
