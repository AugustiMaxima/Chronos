/*
    Library exposing syscall functionalities to user programs
*/

int Create(int priority, void (*function)());

int AwaitEvent(int eventId);

int MyTid();

int MyParentTid();

void Yield();

void Exit();

void Destroy();

int Send(int tid, const char *msg, int msglen, char *reply, int replylen);

int Receive(int* tid, char *msg, int msglen);

int Reply( int tid, const char *reply, int replylen );
