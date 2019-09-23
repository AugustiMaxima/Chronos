#include <ARM.h>
#include <task.h>
#include <bwio.h>

//Allocates, configures and setups the stack layout for context switch
/*
    Some documentation for this function
    The initialization of the stack essentially simulates a involuntary context switch
    Ironically, since we handled both and storing and restoring of the trap frame with stmfd/ldmfd
    This is the only piece of the code where we are exposed to the actual low level details of our trap frame
    And it looks like this:
    CPSR        <---------- Where the stackEntry is pointing to
    R0          <---------- Where the stored SP should point to
    R1
    ...
    SP
    LR
    PC
*/
void initializeStack(Task* task, void* functionPtr){
    bwprintf(COM2, "%x\r\n", task->STACK);
    bwprintf(COM2, "%x\r\n", STACK_SIZE);
    int* stack = task->STACK + STACK_SIZE;

    stack --;
    //PC
    *stack = functionPtr;

    stack--;
    // here lies LR - dont write anything
    *stack = 0xdeadbeef;
    
    stack--;
    task->stackEntry =  (int*)((int)task->STACK + STACK_SIZE) - 17;

    bwprintf(COM2, "Stackentry: %x\r\n", task->stackEntry);
    // set r13 (aka sp)
    *stack = task->stackEntry + 1; //user sp at time of resumption will be missing cpsr

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

    bwprintf(COM2, "CPSR:\t%x\r\n", *(task->stackEntry));
    for(int i=0;i<16;i++){
        bwprintf("R%d:\t%x\r\n", task->stackEntry[i+1]);
    }
}

void initializeTask(Task* task, int tId, int pId, int priority, taskStatus status, void* entryPoint){
    task->tId = tId;
    task->pId = pId;
    task->priority = priority;
    task->status = status;
    int i;
    for(i=0;i<MAX_CHILDREN;i++){
	    task->childTasks[i] = 0;
    }
    initializeStack(task, entryPoint);
}

int appendChildTasks(Task* task, int tId){
    int i;
    for(i=0; i<MAX_CHILDREN; i++){
        if(!task->childTasks[i]){
            task->childTasks[tId];
            return 0;
        }
    }
    return 1;
}
