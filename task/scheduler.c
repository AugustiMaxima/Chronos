#include <ARM.h>
#include <kern.h>
#include <stdlib.h>
#include <scheduler.h>
#include <bwio.h>

extern Scheduler* scheduler;

void initializeScheduler(Scheduler* scheduler){
    initializeQueue(&(scheduler->readyQueue));
    int i;
    for(i=0;i<MAX_TASKS;i++){
	scheduler->tasks[i].tId = 0;
    }
    scheduler->currentTask = NULL;
}

int getAvailableTaskId(Scheduler* scheduler){
    int i;
    for(i=0; i<MAX_TASKS; i++){
	bwprintf(COM2, "%d %d\r\n", i, scheduler->tasks[i].tId);
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
    initializeTask(&(scheduler->tasks[tId-1]), tId, parent, priority);
    bwprintf(COM2, "%x\r\n", scheduler->tasks[tId-1].STACK);
    bwprintf(COM2, "%x\r\n", STACK_SIZE);
    int* stack = scheduler->tasks[tId-1].STACK + STACK_SIZE;
    int* stack_view = stack;
    bwprintf(COM2, "%x\r\n", stack_view);

    stack --;
    //PC
    *stack = functionPtr;

    stack--;
    // here lies LR - dont write anything
    *stack = 0xdeadbeef;

    stack--;
    scheduler->tasks[tId - 1].stackEntry =  (int*)((int)scheduler->tasks[tId-1].STACK + STACK_SIZE) - 17;

    bwprintf(COM2, "Stackentry: %x\r\n", scheduler->tasks[tId - 1].stackEntry);
    // set r13 (aka sp)
    *stack = scheduler->tasks[tId - 1].stackEntry + 1; //user sp at time of resumption will be missing cpsr

    int i;
    // set r0-r12 registers to 0
    for(i=12;i>=0;i--){
        stack--;
        *stack = i;
    }



    stack --;

    int cpsr;

    asm("MRS %0, CPSR" : "=r"(cpsr));

    cpsr &= ~CPSR_M_FLAG;
    cpsr |= CPSR_M_USR;

    //cpsr status, for hardware interrupt capable trap frame
    *stack = cpsr;
    push(&(scheduler->readyQueue), &(scheduler->tasks[tId-1]));

    for(i=1; i<18; i++){
	    bwprintf(COM2, "R%d:\t%x  \t%x\r\n", 16-i,  stack_view-i, stack_view[-i]);
    }
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

void runFirstAvailableTask(Scheduler* scheduler){
    Task* task = pop(&(scheduler->readyQueue));
    runTask(scheduler, task->tId);
}

void runTask(Scheduler* scheduler, int tId){
    exitKernel(scheduler->tasks[tId - 1].stackEntry);
}


void handleSuspendedTasks(){
    void* stackPtr;

    //changes from svc to sys mode
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV %0, SP" ::"r"(stackPtr));

    //changes back to svc
    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");

    scheduler->currentTask->stackEntry = stackPtr;
    //TODO: Figure out and design the blocked queue based on different conditions and status
    //Current iteration : Pretend every suspended task will be ready again right now
    scheduler->currentTask->status = READY;
    push(&(scheduler->readyQueue), scheduler->currentTask);
    scheduler->currentTask = NULL;

    void* jump = enterKernel;

    //jumps here to hand off handling suspended tasks
    asm("mov pc, %0" :"=r"(jump));
}
