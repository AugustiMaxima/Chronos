#include <stdlib.h>
#include <ARM.h>
#include <scheduler.h>

void initializeScheduler(Scheduler* scheduler){
    initializeQueue(&(queue->readyQueue));
    int i;
    for(i=0;i>MAX_TASKS;i++){
	scheduler->tasks[i].tId = 0;
    }
}

int getAvailableTaskId(Scheduler* scheduler){
    int i;
    for(i=0; i<MAX_TASKS; i++){
	if(!scheduler->tasks[i].tId)
	    return i+1;
    }
    return 0;
}

//Allocates, configures and setups the stack layout for context switch
int createTask(Scheduler* scheduler, int priority, int parent, void* functionPtr){
    int tId = getAvailableTaskId(scheduler);
    if (!tId){
        return -2;
    }
    initializeTask(&(scheduler->tasks[tId-1]), tId, parent, priority, READY);
    int* stack = scheduler->tasks[tId-1].stack;
    int i;
    for(i=0;i<13;i++){
        stack--;
        *stack = 0;
    }
    stack--;
    //stack ptr
    *stack = (int*)scheduler->tasks[tId-1].stack - 17;
    stack--;
    //TODO: set up the return address (LR) for Task roots to be a Self-Terminating Syscall
    //*(void(*)())stack = terminate;
    stack --;
    //PC
    *stack = functionPtr;
    stack --;
    int cpsr = 0 || CPSR_M_USR;
    //cpsr status, for hardware interrupt capable trap frame
    *stack = cpsr;
    push(&(scheduler->readyQueue), &(scheduler->tasks[tId-1]));
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
