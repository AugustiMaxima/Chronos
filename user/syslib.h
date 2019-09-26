/*
    Library exposing syscall functionalities to user programs
*/

int createTask(void* functionPtr, int priority);

int getPId();

int getTId();

int yield();

void Exit();