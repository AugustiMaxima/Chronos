#include <ARM.h>
#include <syscode.h>
#include <kern.h>
#include <scheduler.h>
#include <sendReceiveReply.h>
#include <deviceRegistry.h>
#include <syscall.h>
#include <bwio.h>

//We will need to use this scheduler for keeping track of current proc, as well as other important tasks
extern Scheduler* scheduler;
extern COMM* com;
extern DeviceRegistry* registry;

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
            break;
        case SEND_CODE:
            sysSend();
            break;
        case RECEIVE_CODE:
            sysReceive();
            break;
        case REPLY_CODE:
            sysReply();
            break;
        case AWAITEVENT_CODE:
            sysAwaitEvent();
            break;
        default:
            bwprintf(COM2, "Unknown SWI code %d!\r\n", code);
            while(1){}
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

//using R1, because these are less likely to be disturbed
static inline __attribute__((always_inline)) enter_sys_mode() {
    asm("MSR CPSR_c, #"TOSTRING(SYS_MODE));
}

static inline __attribute__((always_inline)) exit_sys_mode() {
    asm("MSR CPSR_c, #"TOSTRING(SVC_MODE));
}

void sysYield(){
}

void sysGetTid(){
    enter_sys_mode();
    asm("MOV R0, SP");
    exit_sys_mode();

    int* sp;
    asm("MOV %0, R0":"=r"(sp));
    sp[1] = scheduler->currentTask->tId;
}

void sysGetPid(){
    enter_sys_mode();
    asm("MOV R0, SP");
    exit_sys_mode();
    int* sp;
    asm("MOV %0, R0":"=r"(sp));
    sp[1] = scheduler->currentTask->pId;
}

void sysAwaitEvent(){
    int* sp;

    //bwprintf(COM2, "Are we still triggered\r\n");

    enter_sys_mode();
    asm("ADD SP, SP, #4");
    asm(R"(
        MOV R0, SP
        MSR CPSR_c, #0x93
        MOV %0, R0
    )": "=r"(sp):: "r0");

    int eventId = sp[-1];

    // if this function is inlined, the system will crash
    
    Task* task = scheduler->currentTask;
    WaitForDevice(registry, task, eventId);
    task->status = BLOCKED;
}


void sysCreateTask(){
    void* funcPtr;
    int priority;
    int* sp;

    enter_sys_mode();
    asm("ADD SP, SP, #8");
    asm("MOV R0, SP");
    exit_sys_mode();

    asm("MOV %0, R0": "=r"(sp));

    priority = sp[-2];
    funcPtr = sp[-1];
    // bwprintf(COM2, "CreateTask priority:%d fptr:%x\r\n",priority,funcPtr);
    int pId = scheduler->currentTask->tId;
    int result = scheduleTask(scheduler, priority, pId, funcPtr);
    //stores the result in the user stack
    sp[1] = result;
}

void sysExit(){
    scheduler->currentTask->status = EXITED;
}

void sysDestroy(){
    scheduler->currentTask->status = EXITED;
    freeTask(scheduler, scheduler->currentTask);
}

void sysSend(){
    int* sp;
    enter_sys_mode();
    asm("ADD SP, SP, #4");
    asm("MOV R0, SP");
    exit_sys_mode();

    asm("MOV %0, R0" : "=r"(sp));
    scheduler->currentTask->stackEntry = sp;

    int* args = sp[-1];

    int tid = args[0];
    char* msg = args[1];
    int msglen = args[2];
    char* rep = args[3];
    int replylen = args[4];

    // bwprintf(COM2, "Send %d %x %d %x %d", tid, msg, msglen, rep, replylen);

    int result = insertSender(com, scheduler->currentTask->tId, tid, msg, msglen, rep, replylen);
    if (result<0){
        sp[1] = result;
    } else {
        scheduler->currentTask->status = BLOCKED;
    }
}

void sysReceive(){
    int* sp;

    enter_sys_mode();
    asm("ADD SP, SP, #4");
    asm("MOV R0, SP");
    exit_sys_mode();

    asm("MOV %0, R0" : "=r"(sp));
    scheduler->currentTask->stackEntry = sp;
    int* args = sp[-1];
    char* msg = args[1];
    int msglen = args[2];
    // bwprintf(COM2, "Receiving %x, %d\r\n", msg, msglen);
    int status = insertReceiver(com, scheduler->currentTask->tId, msg, msglen);
    sp[3] = args[0];
}

void sysReply(){
    int* sp;
    enter_sys_mode();

    asm("ADD SP, SP, #4");
    asm("MOV R0, SP");

    exit_sys_mode();

    asm("MOV %0, R0" : "=r"(sp));

    int* args = sp[-1];
    int tid = args[0];
    char* msg = args[1];
    int msglen = args[2];
    // bwprintf(COM2, "Replying %d, %x, %d", tid, msg, msglen);
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
