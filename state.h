#ifndef STATE_H
#define STATE_H

typedef struct task_state{
    int r0;
    int r1;
    int r2;
    int r3;
    int r4;
    int r5;
    int r6;
    int r7;
    int r8;
    int r9;
    int r10;
    int r11;
    int r12;
    int sp;
    int lr;
    int pc;
    // might need to store cpsr or spsr later 
} State;

#endif

