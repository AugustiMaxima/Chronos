/*
    Library exposing syscall functionalities to user programs
*/

int Create(int priority, void (*function)());

int MyTid();

int MyParentTid();

void Yield();

void Exit();