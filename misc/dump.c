#include <bwio.h>
#include <ts7200.h>
#include <sendReceiveReply.h>
#include <interrupt.h>
#include <nameServer.h>

void printSp() {
    unsigned int sp;
    asm volatile("mov %0, sp" : "=r" (sp));

    bwputstr(COM2, "sp=");
    bwputr(COM2, sp);
    bwputstr(COM2, "\r\n");
}

void printCurrentMode() {
    unsigned int cpsr;
    asm volatile("mrs %0, cpsr" : "=r" (cpsr));

    cpsr = cpsr & 0x1F; // get last 5 bits

    if (cpsr == 0x10) {
        bwprintf(COM2, "user mode\r\n");
    } else if (cpsr == 0x11) {
        bwprintf(COM2, "fiq mode\r\n");
    } else if (cpsr == 0x12) {
        bwprintf(COM2, "irq mode\r\n");
    } else if (cpsr == 0x13) {
        bwprintf(COM2, "supervisor mode\r\n");
    } else if (cpsr == 0x17) {
        bwprintf(COM2, "abort mode\r\n");
    } else if (cpsr == 0x1B) {
        bwprintf(COM2, "undefined mode\r\n");
    } else if (cpsr == 0x1F) {
        bwprintf(COM2, "system mode\r\n");;
    } else {
        // we read something in the mode bits that's not supposed to be there
        bwprintf(COM2, "illegal mode\r\n");
    }
}

void printCpsrI() {
    // print the I bit of CPSr
    unsigned int cpsr;
    asm volatile("mrs %0, cpsr" : "=r" (cpsr));

    cpsr = cpsr & 0x80; // get 7th bit

    if (cpsr == 0) {
        bwprintf(COM2, "interrupts enabled\r\n");
    } if (cpsr == 0x80) {
        bwprintf(COM2, "interrupts disabled\r\n");
    }
}

void enableCpsrI() {
    unsigned int cpsr;
    asm volatile("mrs %0, cpsr" : "=r" (cpsr));
    cpsr = cpsr & ~0x80;
    asm("MSR CPSR, %0" :: "r" (cpsr));
}

void warnAtEndOfKernel(COMM* com) {

    int enabledMask1 = *(volatile unsigned*)(VIC1ADDR + VIC_ENABLE);
    int enabledMask2 = *(volatile unsigned*)(VIC2ADDR + VIC_ENABLE);

    if (enabledMask1 || enabledMask2) {
        bwprintf(COM2, "PANIC: interrupts enabled at end of kernel\r\n");
        while (1) {}
    }

    Node* node = 0;
    do {
        node = iterateMap(&(com->senderRequestTable), node);
        Receiver* receiver = (Receiver*) node;
        if (node) bwprintf(COM2, "Warning: TID %d blocked because it executed a send but target task has not called receive\r\n", receiver->tId);
    } while(node!=0);

    node = 0;
    do {
        node = iterateMap(&(com->receiverTable), node);
        Receiver* receiver = (Receiver*) node;
        if (node && receiver->tId != getNsTid()) bwprintf(COM2, "Warning: TID %d blocked on receive\r\n", receiver->tId);
    } while(node!=0);

    node = 0;
    do {
        node = iterateMap(&(com->senderReplyTable), node);
        Receiver* receiver = (Receiver*) node;
        if (node) bwprintf(COM2, "Warning: TID %d blocked because it executed a send but target task has not called reply\r\n", receiver->tId);
    } while(node!=0);
}
