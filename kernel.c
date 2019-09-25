 /*
 * kernel.c
 *
 * Minimal code for developing context switch
 */

#include <bwio.h>
#include <ts7200.h>
#include <dump.h>
#include <scheduler.h>
#include <task.h>
#include <ARM.h>

Scheduler* scheduler;
void* stackPtr;

void __attribute__((naked)) handle_swi () {
    bwprintf(COM2, "handle_swi\r\n");

    //changes from svc to sys mode
    asm("MRS R0, CPSR");
    asm("ADD R0, R0, #12");
    asm("MSR CPSR, R0");

    asm("MOV %0, SP" :"=r"(stackPtr));
    bwprintf(COM2, "sp=%x\r\n", stackPtr);

    //changes back to svc
    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");
    
    asm("LDR R0, [SP]");
    asm("LDR R1, [SP, #4]");
    asm("LDR R2, [SP, #8]");
    asm("LDR R3, [SP, #12]");
    asm("LDR R4, [SP, #16]");
    asm("LDR R5, [SP, #20]");
    asm("LDR R6, [SP, #24]");
    asm("LDR R7, [SP, #28]");
    asm("LDR R8, [SP, #32]");
    asm("LDR R9, [SP, #36]");
    asm("LDR R10, [SP, #40]");
    asm("LDR R11, [SP, #44]");
    asm("LDR R12, [SP, #48]");
    asm("LDR R13, [SP, #52]");
    asm("LDR R14, [SP, #56]");
    asm("ADD SP, SP, #64");
    asm("LDR R15, [SP, #-4]");

    asm("mov pc, lr");
}

void* yield() {
    bwprintf(COM2, "yielding\r\n");
    asm("MOV %0, SP" :"=r"(stackPtr));

    asm("SUB SP, SP, #64");
    asm("STR R0, [SP]");
    asm("STR R1, [SP, #4]");
    asm("STR R2, [SP, #8]");
    asm("STR R3, [SP, #12]");
    asm("STR R4, [SP, #16]");
    asm("STR R5, [SP, #20]");
    asm("STR R6, [SP, #24]");
    asm("STR R7, [SP, #28]");
    asm("STR R8, [SP, #32]");
    asm("STR R9, [SP, #36]");
    asm("STR R10, [SP, #40]");
    asm("STR R11, [SP, #44]");
    asm("STR R12, [SP, #48]");
    asm("STR R13, [SP, #52]");
    asm("STR R14, [SP, #56]");
    asm("ADD R2, PC, #16");
    //sets where the return address should be
    asm("STR R2, [SP, #60]");
    asm("SUB SP, SP, #4");


    asm("MRS R2, CPSR");
    asm("STR R2, [SP]");
    asm("swi");

    //<---- PC return points here
    bwprintf(COM2, "yielded\r\n");
}

void* first() {
    bwprintf(COM2, "First task\r\n");
    asm("mov r4, #0x42");
    printCurrentMode();
    printSp();
}

void* call_user_task() {
    first();
    yield();
}

void printSavedContext(int* sp) {
    bwprintf(COM2, "printSavedContext\r\n");
    int i;
    for (i = 0; i < 17; i++) {
        bwprintf(COM2, "%x\t\t%x\r\n", sp + i, *(sp + i));
    }
}

int main( int argc, char* argv[] ) {

	bwsetfifo(COM2, OFF);
    installSwiHandler(handle_swi);

    Scheduler scheduler;
    initializeScheduler(&scheduler);

    int k = scheduleTask(&scheduler, 0, 0, call_user_task);

    if (k==-2){
	bwprintf(COM2, "You dun goofed");
    }

    int firstTid = getFirstAvailableTask(&scheduler);

    bwprintf(COM2, "runTask\r\n");
    printSp();

    // the following lines are not interchangeable

    //runTask(&scheduler, firstTid);
    exitKernel(scheduler.tasks[firstTid - 1].stackEntry);

    bwprintf(COM2, "runTask done\r\n");
    printSp();

    exitKernel(stackPtr);
    
    bwprintf(COM2, "Function finished?\r\n");

	return 0;
}
