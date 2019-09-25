#include <ARM.h>
#include <kern.h>
#include <scheduler.h>
#include <syscall.h>

//We will need to use this scheduler for keeping track of current proc, as well as other important tasks
extern Scheduler* scheduler;

//TODO: Consider switching this logic to jump table
__attribute__((naked)) void sys_handler(){
    
    int code;
    asm("ldr %0, [LR, #-4]" :"=r"(code));

    code &= SWI_OPCODE_MASK;

    switch (code){
        case 0:
            getTid();
        case 1:
            sysCreateTask();
        default:
    }
    
    void* jump = handleSuspendedTasks;

    //jumps here to hand off handling suspended tasks
    asm("mov pc, %0" :"=r"(jump));
}

__attribute__((naked)) void getPid(){

}

__attribute__((naked)) void getTid(){
    int tId = scheduler->currentTask->tId;
    scheduler->currentTask->stackEntry[1] = tId;
    asm("mov pc, lr");
}

__attribute__((naked)) void sysCreateTask(){
    void* funcPtr;
    int priority;
    //goes into system mode, in order to unwind the stack and retrieve the additional arguments
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    //arguments are stored and retrieved in ascending order
    asm("LDR %1, [SP]", :: "r" (funcPtr));
    asm("LDR %1, [SP, 4]", :: "r" (priority));
    asm("ADD SP, SP, #8");
    
    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    int pId = scheduler->currentTask->tId;
    scheduleTask(scheduler, priority, pId, funcPtr);
}