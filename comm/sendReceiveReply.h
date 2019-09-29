#ifndef SEND_RECEIVE_REPLY_H
#define SEND_RECEIVE_REPLY_H

#include <queue.h>
#include <map.h>

//Concepts around interprocess communications
//Sender: requests and checks for the existence of target tId (or receiverConstruct), otherwise exits
//Receiver: checks for the existence of a valid senderConstruct with the right tId, sleeps

#define MAX_SENDER 64
#define MAX_RECEIVER 64

typedef struct senderConstruct{
    int tId;
    int requestTId;
    char* source;
    int sourceLength;
    char* receiveBuffer;
    int receiveLength;
} Sender;

typedef struct receiverConstruct{
    int tId;
    char* receiveBuffer;
    int receiveLength;
} Receiver;


typedef struct SendReceiveReply{
    Sender sendPool[MAX_SENDER];
    Receiver receivePool[MAX_RECEIVER];
    Queue sendQueue;
    Queue receiveQueue;
    Map senderRequestTable;
    Map receiverTable;
    Map senderReplyTable;
} COMM;

void initializeCOMM(COMM* com);

int insertSender(COMM* com, int tId, int requestTId, const char* source, int length, char* receive, int rlength);

int insertReceiver(COMM* com, int tId, char* receive, int length);

int reply(COMM* com, const char* reply, int length, int tId);

#endif
