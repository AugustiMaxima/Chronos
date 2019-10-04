#include <map.h>
#include <charay.h>
#include <syslib.h>
#include <stdlib.h>
#include <bwio.h>

#define MAX_REQUEST 100
#define MAX_RESULT 100

int Registration(Map* NameTable, char* symbol, int tId){
    return putMap(NameTable, alphaNumericHash(symbol), (void*)tId);
}

int Retrieve(Map* NameTable, char* symbol){
    return (int) getMap(NameTable, alphaNumericHash(symbol));
}

void RegistrationPreamble(Map* NameTable, char* symbol, int caller){
    int returnCode = Registration(NameTable, symbol, caller);
    if(returnCode == 1)
        Reply(caller, "Registration successful.", MAX_RESULT);
    else if(returnCode == 0)
        Reply(caller, "Updated registration", MAX_RESULT);
    else
        Reply(caller, "Register failed due to insufficent resources", MAX_RESULT);
}


void RetrievalPreamble(Map* NameTable, char* symbol, int caller){
    int returnCode = Retrieve(NameTable, symbol);
    if(!returnCode){
        Reply(caller, "Registration not found", MAX_RESULT);
    } else {
        char numeral[MAX_RESULT];
        noneZeroIntString(numeral, MAX_RESULT, returnCode, 10);
        Reply(caller, numeral, MAX_RESULT);
    }
}

extern int nsTid;

void nameServer(){
    Map NameTable;
    initializeMap(&NameTable);
    int caller;
    char requestBuf[MAX_REQUEST];
    char command;
    char* symbol;

    nsTid = MyTid();
    while(Receive(&caller, requestBuf, MAX_REQUEST)){
        if (0 == strlcmp(requestBuf, "kys")) {
            Reply(caller, "ok", strlen("ok"));
            Exit();
        }
        command = requestBuf[0];
        symbol = requestBuf;
        symbol += 2;
        if(command == 'R'){
	    bwprintf(COM2, "Receiving registration %d, %s \r\n", caller, symbol);
            RegistrationPreamble(&NameTable, symbol, caller);
        } else if(command == 'W'){
            RetrievalPreamble(&NameTable, symbol, caller);
        } else {
            Reply(caller, "Bad operation", MAX_RESULT);
        }
    }
}

int getNsTid() {
    if (nsTid == -1) {
        bwprintf(COM2, "nameserver not up yet\r\n");
        for (;;) {}
    }
    return nsTid;
}

int RegisterAs(const char *name){
    char buffer[100];
    char receiveBuffer[100];
    formatStrn(buffer, 100, "R %s", name);
    int result = Send(getNsTid(), buffer, 100, receiveBuffer, 100);
    if(result>0){
        return 0;
    } else {
        return -1;
    }
}

int WhoIs(const char *name){
    char buffer[100];
    char receiveBuffer[100];
    formatStrn(buffer, 100, "W %s", name);
    int result = Send(getNsTid(), buffer, 100, receiveBuffer, 100);
    if(result>0){
        if(strlcmp("Registration not found", receiveBuffer))
            return stringToNum(receiveBuffer, 10);
        else
            return 0;
    } else {
        return -1;
    }
}
