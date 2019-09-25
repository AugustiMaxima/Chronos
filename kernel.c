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

    asm("MOV %0, SP" ::"r"(stackPtr));
    bwprintf(COM2, "sp=%x\r\n", stackPtr);

    //changes back to svc
    asm("MRS R0, CPSR");
    asm("SUB R0, R0, #12");
    asm("MSR CPSR, R0");


    asm("LDMFD SP!, {R0-R12, LR}");
    asm("mov pc, lr");
}

void* yield() {
    bwprintf(COM2, "yielding\r\n");
    asm("MOV %0, SP" :"=r"(stackPtr));

    // asm("STMFD SP, {R0-PC}");
    // asm("MOV R2, PC"); // <--------------- This is the point where the PC is stored, count downwards from here
    // asm("ADD R2, r2, #24");
    // asm("STR R2, [SP, #60]");
    // asm("SUB SP, SP, #4");


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

void* __attribute__((naked)) call_user_task() {
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

    runTask(&scheduler, firstTid);
    // exitKernel(scheduler.tasks[firstTid - 1].stackEntry);

    bwprintf(COM2, "runTask done\r\n");
    printSp();



	return 0;
}
