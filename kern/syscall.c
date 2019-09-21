#include <scheduler.h>
#include <syscall.h>

//We will need to use this scheduler for keeping track of current proc, as well as other important tasks
extern Scheduler* scheduler;

//TODO: Consider switching this logic to jump table
void sys_handler(int code){

    //First order of business: Update current StackPtr
    asm("ADD R0, R0, #12");

    switch (code){
        case 0:
            getPid();
        case 1:
            createTask();
        default:
    }
}

void getPid(){

}

void createTask(){
    void* funcPtr;
    int priority;

    //goes into system mode, in order to unwind the stack and retrieve the additional arguments
    asm("MRS R0, CPSR");
    //12 is the distance from svc to sys mode
    asm("ADD R0, R0, #12");

    //R3 for first argument, the function pointer
    //R4 for second argument, the priority 
    asm("ldr %1, [SP]", :: "r" (funcPtr));
    asm("add SP, SP, #4");
    asm("ldr %1, [SP]", :: "r" (priority));
    asm("add SP, SP, #4");
    
    int pId = 
}