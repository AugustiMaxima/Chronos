#include <ARM.h>
#include <kern.h>
#include <stdlib.h>
#include <scheduler.h>
#include <bwio.h>

extern Scheduler* scheduler;

int taskID;

void printRegisters(int* stack){
    bwprintf(COM2, "Stack pointer : %x\r\n", stack);
    bwprintf(COM2, "CPSR [%x]: %x\r\n", stack, *stack);
    stack++;
    int i=0;
    for(i=0;i<16;i++){
        bwprintf(COM2, "R%d [%x]: %x\r\n", i, stack, *stack);
        stack++;
    }
}

void printTask(Task* task){
    // bwprintf(COM2, "Entering Task: %d\r\n", task->tId);
    // printRegisters(task->stackEntry);
}

void initializeScheduler(Scheduler* scheduler){
    taskID = 1;
    intializePriorityQueue(&(scheduler->readyQueue));
    initializeQueue(&(scheduler->freeQueue));
    initializeMap(&(scheduler->taskTable));
    int i;
    for(i=0;i<MAX_TASKS;i++){
	    push(&(scheduler->freeQueue), scheduler->tasks + i);
    }
    scheduler->currentTask = NULL;
}

//Allocates, configures and setups the stack layout for context switch
int scheduleTask(Scheduler* scheduler, int priority, int parent, void* functionPtr){
    Task* task = pop(&(scheduler->freeQueue));
    if (!task){
        return -2;
    }
    int tId = taskID++;
    initializeTask(task, tId, parent, priority, READY, functionPtr);
    insertMap(&(scheduler->taskTable), tId, task);
    insertTaskToQueue(scheduler, task);
    return tId;
}

void freeTask(Scheduler* scheduler, Task* task){
    push(&(scheduler->freeQueue), task);
}

int runFirstAvailableTask(Scheduler* scheduler) {
    Task* task;
    task = removeMin(&(scheduler->readyQueue));
    if(task){
        runTask(scheduler, task);
        return 0;
    } else{
        return -1;
    }
}

void runTask(Scheduler* scheduler, Task* task){
    scheduler->currentTask = task;
    task->status = RUNNING;
    printTask(task);
    exitKernel(task->stackEntry);
    // asm("MOV R0, #1");
    // asm("MOV R1, SP");
    // asm("BL bwputr(PLT)");
    // bwprintf(COM2, "end of runTask\r\n");
}

int insertTaskToQueue(Scheduler* scheduler, Task* task){
    task->status = READY;
    return insert(&(scheduler->readyQueue), task);
}

//TODO: Delete entries from taskTable when they are destroyed
Task* getTask(Scheduler* schedule, int tId){
    return getMap(&(schedule->taskTable), tId);
}


void handleSuspendedTasks(void* lr){
    int* stackPtr;
    //changes from svc to sys mode
    asm("MRS R3, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R3, R3, #12");
    asm("MSR CPSR, R3");

    asm("MOV R1, SP");

    //changes back to svc
    asm("MRS R3, CPSR");
    asm("SUB R3, R3, #12");
    asm("MSR CPSR, R3");
    asm volatile("MOV %0, R1":"=r"(stackPtr));

    // bwprintf(COM2, "\r\n");
    // bwprintf(COM2, "Scheduler Ptr: %x\r\n", scheduler);
    // bwprintf(COM2, "acquired stackPtr: %x\r\n", stackPtr);

    stackPtr[16] = lr;    
    scheduler->currentTask->stackEntry = stackPtr;
    //TODO: Figure out and design the blocked queue based on different conditions and status
    // Current iteration : Pretend every suspended task will be ready again right now
    if(scheduler->currentTask->status == RUNNING){
        int code = insertTaskToQueue(scheduler, scheduler->currentTask);
    }
    scheduler->currentTask = NULL;

    // bwprintf(COM2,"User program halt, trapframe printing!\r\n");
    // printRegisters(stackPtr);
}
