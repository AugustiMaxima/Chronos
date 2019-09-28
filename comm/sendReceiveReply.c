#include <sendReceiveReply.h>

void initializeSender(Sender* sender){
    sender->tId = 0;
    sender->requestTId = 0;
}

void intializeReceiver(Receiver* receiver){
    receiver->tId = 0;
}

void initializeCOMM(COMM* com){
    int i=0;
    for(i=0; i<MAX_SENDER; i++){
	initializeSender(com->sendPool + i);
    }
    for(i=0; i<MAX_RECEIVER; i++){
	initializeReceiver(com->receivePool + i);
    }
    initializeMap(&(com->senderTable));
    initializeMap(&(com->receiverTable));
}

int processSender(COMM* com){
    
}

int insertSender(COMM* com, int tId, int requestTId){
    int i=0;
    for(i=0; i<MAX_SENDER; i++){
	if(com->sendPool[i].tId == 0){
	    com->sendPool[i].tId = tId;
	    com->sendPool[i].requestTId = requestTId;
	    return processSender(com);
	}
    }
    return -1;
}

int insertReceiver(COM* com, int tId){
    int i=0;
    for(i=0; i<MAX_RECEIVER; i++){
	if(com->receivePool[i].tId == 0){
	    com->receivePool[i].tId = tId;
	    insertMap(&(com->receiveTable), tId, com->sendPool + i);
	}
    }
}
