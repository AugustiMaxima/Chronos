#include <ARM.h>
#include <task.h>
#include <syslib.h>
#include <bwio.h>


/*
    TrapFrameLayout:
    CPSR    <-- StackEntry
    R0      <-- SP stored here
    R1
    ...
    R15
*/
void initializeStack(Task* task, void* functionPtr){

    char* stack_base = task->STACK;
    // debug only; separate user and kernel stack by large offsets

    int* stack = stack_base + STACK_SIZE;
    int* stack_view = stack;

    stack --;
    //PC
    *stack = functionPtr;

    stack--;

    //TODO: Set up exit handler
    *stack = Exit;

    //stack--;

    task->stackEntry =  (int*)((int)stack_base + STACK_SIZE) - 16;

    // set r13 (aka sp)
    //*stack = task->stackEntry + 1; //user sp at time of resumption will be missing cpsr

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
}

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status, void* functionPtr){
    task->tId = tId;
    task->pId = pId;
    task->priority = priority;
    task->status = status;
    initializeStack(task, functionPtr);
}
