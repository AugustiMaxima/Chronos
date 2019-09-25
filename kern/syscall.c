#include <kern.h>
#include <scheduler.h>
#include <syscall.h>
#include <bwio.h>

//We will need to use this scheduler for keeping track of current proc, as well as other important tasks
extern Scheduler* scheduler;

//TODO: Consider switching this logic to jump table
void __attribute__((naked)) sys_handler(){

    //First order of business: Update current StackPtr
    int code;
    asm("LDR %0, [LR, #-4]":"=r"(code));

    switch (code){
        case 0:
            sysYield();
        case 1:
            sysCreateTask();
        default:
            ;
    }
    
    void* jump = handleSuspendedTasks;

    //jumps here to hand off handling suspended tasks
    asm("mov pc, %0" :"=r"(jump));
}

void sysYield(){
    bwprintf(COM2, "%s", "Yielding!\r\n");
}

void sysGetTid(){}

void sysGetPid(){}


void sysCreateTask(){
    void* funcPtr;
    int priority;
    //goes into system mode, in order to unwind the stack and retrieve the additional arguments
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    //arguments are stored and retrieved in ascending order
    asm("LDR %0, [SP]" :"=r"(funcPtr));
    asm("LDR %0, [SP, #4]" :"=r"(priority));
    asm("ADD SP, SP, #8");
    
    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    int pId = scheduler->currentTask->tId;
    int result = scheduleTask(scheduler, priority, pId, funcPtr);
    //stores the result in the user stack
    asm("STR %0, [R2, #4]"::"r"(result));
}

void setUpSWIHandler(void* handle_swi) {
    /*
    Install SWI handler
    The swi instruction executes at address 0x8. The following two lines write
    this to the memory addresses:

    0x08        LDR pc, [pc, #0x18]
    0x0c        ?
    ...         ?
    0x28        <absolute address of handle_swi>

    Note that ARM prefetches 2 instructions ahead. Hence, after a software
    interrupt, instruction 0x08 executes with pc=0x10.
    */
    *((unsigned*)0x8) = 0xe59ff018;
    *((unsigned*)0x28) = handle_swi;
}