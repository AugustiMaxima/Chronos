#ifndef SEND_RECEIVE_REPLY_H
#define SEND_RECEIVE_REPLY_H

#include <map.h>

//Concepts around interprocess communications
//Sender: requests and checks for the existence of target tId (or receiverConstruct), otherwise exits
//Receiver: checks for the existence of a valid senderConstruct with the right tId, sleeps

#define MAX_SENDER 64
#define MAX_RECEIVER 64

typedef struct senderConstruct{
    int tId;
    int requestTId;
} Sender;

typedef struct receiverConstruct{
    int tId;   
} Receiver;


typedef struct SendReceiveReply{
    Sender sendPool[MAX_SENDER];
    Receiver receivePool[MAX_RECEIVER];
    Map senderTable;
    Map senderRequestTable;
    Map receiverTable;
} COMM;

void initializeCOMM(COMM* com);

int insertSender(COMM* com, int tId, int requestTid);

int insertReceiver(COMM* com, int tId);

int reply(COMM* com, int replyTid, int tId);

int SendMsg(COMM* com, int tId, int requestTid);

int replyMsg(COMM* com, int replyTId, int tId);

#endif
