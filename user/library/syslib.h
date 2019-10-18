/*
    Library exposing syscall functionalities to user programs
*/

int Create(int priority, void (*function)());

int MyTid();

int MyParentTid();

void Yield();

void Exit();

void Destroy();

int Send(int tid, const char *msg, int msglen, char *reply, int replylen);

int Receive(int* tid, char *msg, int msglen);

int Reply( int tid, const char *reply, int replylen );

int AwaitEvent(int eventId);

int AwaitMultipleEvent(int* val, int deviceCount, ...);
