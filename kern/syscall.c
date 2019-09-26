#include <ARM.h>
#include <kern.h>
#include <scheduler.h>
#include <syscall.h>
#include <bwio.h>

//We will need to use this scheduler for keeping track of current proc, as well as other important tasks
extern Scheduler* scheduler;

//handles the switch statements
void jumpTable(int code){
    code &= SWI_OPCODE_FLAG;
    switch (code){
        case 0:
            sysYield();
            break;
        case 1:
            sysCreateTask();
            break;
        case 4:
            sysExit();
            break;
        default:
            ;
    }
}

//TODO: Consider switching this logic to jump table
void __attribute__((naked)) sys_handler(){
    asm("LDR R0, [LR, #-4]");

    asm("BL jumpTable");

    //jumps here to hand off suspendedTask resumptions    
    asm("BL handleSuspendedTasks");

    asm("LDR LR, =enterKernel");
    asm("MOV PC, LR");
}

void sysYield(){
    bwprintf(COM2, "%s", "Yielding!\r\n");
}

void sysGetTid(){}

void sysGetPid(){}


void sysCreateTask(){
    void* funcPtr;
    int priority;
    int* sp;
    //goes into system mode, in order to unwind the stack and retrieve the additional arguments
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("ADD SP, SP, #8");

    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    
    asm("MOV %0, R2": "=r"(sp));
    funcPtr = sp[-2];
    priority = sp[-1];

    int pId = scheduler->currentTask->tId;
    int result = scheduleTask(scheduler, priority, pId, funcPtr);
    //stores the result in the user stack
    sp[1] = result;
}

void sysExit(){
    Scheduler->currentTask->status = EXITED;
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