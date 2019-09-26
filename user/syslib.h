/*
    Library exposing syscall functionalities to user programs
*/

int CreateF(void* functionPtr, int priority);

int MyTid();

int MyParentTid();

int yield();

void Exit();