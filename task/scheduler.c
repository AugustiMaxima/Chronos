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
    bwprintf(COM2, "Entering Task: %d\r\n", task->tId);
    printRegisters(task->stackEntry);
}

void initializeScheduler(Scheduler* scheduler){
    initializeQueue(&(scheduler->readyQueue[0]));
    initializeQueue(&(scheduler->readyQueue[1]));
    initializeQueue(&(scheduler->readyQueue[2]));
    int i;
    for(i=0;i<MAX_TASKS;i++){
	    scheduler->tasks[i].tId = 0;
    }
    scheduler->currentTask = NULL;
}

int getAvailableTaskId(Scheduler* scheduler){
    int i;
    for(i=0; i<MAX_TASKS; i++){
	// bwprintf(COM2, "%d %d\r\n", i, scheduler->tasks[i].tId);
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

    insertTaskToQueue(scheduler, &(scheduler->tasks[tId-1]));

    return 0;
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

int getFirstAvailableTask(Scheduler* scheduler) {
    Task* task = peep(&(scheduler->readyQueue));
    return task->tId;
}

void runFirstAvailableTask(Scheduler* scheduler) {
    Task* task = NULL;
    int i=0;
    for(i=0;i<3 & !task;i++){
        task = pop(&scheduler->readyQueue[i]);
    }
    if(task){
        runTask(scheduler, task->tId);
    } else{
        bwprintf(COM2, "No Task Available!\r\n");
    }
}

void runTask(Scheduler* scheduler, int tId){
    Task* task = &(scheduler->tasks[tId - 1]);
    scheduler->currentTask = task;    
    task->status = RUNNING;
    printTask(task);
    exitKernel(task->stackEntry);
    // bwprintf(COM2, "end of runTask\r\n");
}

int insertTaskToQueue(Scheduler* scheduler, Task* task){
    if(task->priority>0){
        push(&(scheduler->readyQueue[0]), task);
    }
    if(task->priority==0){
        push(&(scheduler->readyQueue[1]), task);
    }
    if(task->priority<0){
        push(&(scheduler->readyQueue[2]), task);
    }
}

void handleSuspendedTasks(){
    void* stackPtr;
    //changes from svc to sys mode
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    // asm("MOV R0, #1");
    // asm("MOV R1, SP");
    // asm("BL bwputr(PLT)");
    asm("MOV R3, SP");

    //changes back to svc
    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    asm("MOV %0, R3":"=r"(stackPtr));

    // bwprintf(COM2, "\r\n");
    // bwprintf(COM2, "Scheduler Ptr: %x\r\n", scheduler);
    // bwprintf(COM2, "acquired stackPtr: %x\r\n", stackPtr);

    scheduler->currentTask->stackEntry = stackPtr;
    //TODO: Figure out and design the blocked queue based on different conditions and status
    // Current iteration : Pretend every suspended task will be ready again right now
    if(scheduler->currentTask->status == RUNNING){
        scheduler->currentTask->status == READY;
        int code = insertTaskToQueue(scheduler, scheduler->currentTask);
    }
    scheduler->currentTask = NULL;

    bwprintf(COM2,"User program halt, trapframe printing!\r\n");
    printRegisters(stackPtr);
}
