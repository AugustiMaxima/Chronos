#include <ARM.h>
#include <syscode.h>
#include <kern.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <syscall.h>
#include <bwio.h>

//We will need to use this scheduler for keeping track of current proc, as well as other important tasks
extern Scheduler* scheduler;
extern COMM* com;

//handles the switch statements
void jumpTable(int code){
    code &= SWI_OPCODE_FLAG;
    switch (code){
        case YIELD_CODE:
            sysYield();
            break;
        case CREATE_CODE:
            sysCreateTask();
            break;
        case MYTID_CODE:
            sysGetTid();
            break;
        case MYPARENTTID_CODE:
            sysGetPid();
            break;
        case EXIT_CODE:
            sysExit();
            break;
        case DESTROY_CODE:
            sysDestroy();
        default:
            bwprintf(COM2, "%s", "Miss %d!\r\n", code);
    }
}

//TODO: Consider switching this logic to jump table
void __attribute__((naked)) sys_handler(){
    asm("SUB SP, SP, #4");
    asm("STR LR, [SP]");
    asm("LDR R0, [LR, #-4]");

    asm("BL jumpTable");

    //jumps here to hand off suspendedTask resumptions
    asm("LDR R0, [SP]");
    asm("ADD SP, SP, #4");
    asm("BL handleSuspendedTasks");

    asm("LDR LR, =enterKernel");
    asm("MOV PC, LR");
}

void sysYield(){
    //bwprintf(COM2, "%s", "Yielding!\r\n");
}

void sysGetTid(){
    //bwprintf(COM2, "%s", "getTid!\r\n");
    // bwprintf(COM2, "%s", "getting tid\r\n");
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    int* sp;
    asm("MOV %0, R2":"=r"(sp));
    sp[1] = scheduler->currentTask->tId;
}

void sysGetPid(){
    //bwprintf(COM2, "%s", "GetPid!\r\n");
    // bwprintf(COM2, "%s", "getting pid\r\n");
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    int* sp;
    asm("MOV %0, R2":"=r"(sp));
    sp[1] = scheduler->currentTask->pId;
}


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
    priority = sp[-2];
    funcPtr = sp[-1];

    int pId = scheduler->currentTask->tId;
    int result = scheduleTask(scheduler, priority, pId, funcPtr);
    //stores the result in the user stack
    sp[1] = result;
}

void sysExit(){
    //bwprintf(COM2, "%s", "Exit!\r\n");
    scheduler->currentTask->status = EXITED;
}

void sysDestroy(){

}

void sysSend(){
    int* sp;
    asm("MRS R0, CPSR");
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("ADD SP, SP, #20");
    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV %0, R2" : "=r"(sp));

    scheduler->currentTask->stackEntry = sp;
    int tid = sp[-5];
    char* msg = sp[-4];
    int msglen = sp[-3];
    char* reply = sp[-2];
    int replylen = sp[-1];
    int result = insertSender(com, scheduler->currentTask->tId, tid, msg, msglen, reply, replylen);
    if (result<0){
        sp[1] = result;
    } else {
        scheduler->currentTask = BLOCKED;
    }
}

void sysReceive(){
    int* sp;
    asm("MRS R0, CPSR");
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("ADD SP, SP, #12");
    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV %0, R2" : "=r"(sp));

    char* msg = sp[-2];
    int msglen = sp[-1];

    int status = insertReceiver(com, scheduler->currentTask->tId, msg, msglen);
}

void sysReply(){
        int* sp;
    asm("MRS R0, CPSR");
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("ADD SP, SP, #12");
    asm("MOV R2, SP");

    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV %0, R2" : "=r"(sp));

    int tid = sp[-3];
    char* msg = sp[-2];
    int msglen = sp[-1];
    int result = reply(com, msg, msglen, tid);
    sp[1] = result;
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