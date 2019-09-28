#include <ARM.h>
#include <kern.h>
#include <stdlib.h>
#include <scheduler.h>
#include <bwio.h>

extern Scheduler* scheduler;

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
    intializePriorityQueue(&(scheduler->readyQueue));
    int i;
    for(i=0;i<MAX_TASKS;i++){
        scheduler->tasks[i].tId = 0;
    }
    scheduler->currentTask = NULL;
}

int getAvailableTaskId(Scheduler* scheduler){
    int i;
    for(i=0; i<MAX_TASKS; i++){
        if(scheduler->tasks[i].tId == 0)
            return i+1;
    }
    return 0;
}

//Allocates, configures and setups the stack layout for context switch
int scheduleTask(Scheduler* scheduler, int priority, int parent, void* functionPtr){
    int tId = getAvailableTaskId(scheduler);
    if (!tId){
        return -2;
    }
    initializeTask(&(scheduler->tasks[tId-1]), tId, parent, priority, READY, functionPtr);

    insertTaskToQueue(scheduler, &(scheduler->tasks[tId - 1]));

    return tId;
}

void freeTask(Scheduler* scheduler, int tId){
    int i;
    for(i=0;i<MAX_CHILDREN;i++){
        int cId = scheduler->tasks[tId-1].childTasks[i];
        if(cId){
            scheduler->tasks[cId-1].pId = 0;
        }
        scheduler->tasks[tId-1].childTasks[i] = 0;
    }
    scheduler->tasks[tId-1].tId = 0;
}

int runFirstAvailableTask(Scheduler* scheduler) {
    Task* task;
    int i=0;
    task = removeMin(&(scheduler->readyQueue));
    if(task){
        runTask(scheduler, task->tId);
        return 0;
    } else{
        return -1;
    }
}

void runTask(Scheduler* scheduler, int tId){
    Task* task = &(scheduler->tasks[tId - 1]);
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
    return insert(&(scheduler->readyQueue), task);
}

Task* getTask(Scheduler* scheduler, int tId){
    return &(scheduler->tasks[tId - 1]);
}


void handleSuspendedTasks(void* lr){
    int* stackPtr;
    //changes from svc to sys mode
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV R0, SP");

    //changes back to svc
    asm("MRS R3, CPSR");
    asm("SUB R3, R3, #12");
    asm("MSR CPSR, R3");
    asm("MOV %0, R0":"=r"(stackPtr));

    // bwprintf(COM2, "\r\n");
    // bwprintf(COM2, "Scheduler Ptr: %x\r\n", scheduler);
    // bwprintf(COM2, "acquired stackPtr: %x\r\n", stackPtr);

    scheduler->currentTask->stackEntry = stackPtr;
    stackPtr[16] = lr;
    //TODO: Figure out and design the blocked queue based on different conditions and status
    // Current iteration : Pretend every suspended task will be ready again right now
    if(scheduler->currentTask->status == RUNNING){
        scheduler->currentTask->status == READY;
        int code = insertTaskToQueue(scheduler, scheduler->currentTask);
    }
    scheduler->currentTask = NULL;

    // bwprintf(COM2,"User program halt, trapframe printing!\r\n");
    // printRegisters(stackPtr);
}
