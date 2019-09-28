#ifndef SEND_RECEIVE_REPLY_H
#define SEND_RECEIVE_REPLY_H


//Concepts around interprocess communications
//Sender: requests and checks for the existence of target tId (or receiverConstruct), otherwise exits
//Receiver: checks for the existence of a valid senderConstruct with the right tId, sleeps

typedef struct senderConstruct{
    int tId;
    int requestTId;
} Sender;

typedef struct receiverConstruct{
    int tId;
} Receiver;



#endif