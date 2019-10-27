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
        Reply(caller, "Registration successful", 24);
    else if(returnCode == 0)
        Reply(caller, "Updated registration", 21);
    else
        Reply(caller, "Registeration failed", 20);
}


void RetrievalPreamble(Map* NameTable, char* symbol, int caller){
    int returnCode = Retrieve(NameTable, symbol);
    if(!returnCode){
        Reply(caller, "Not found", strlen("Not found"));
    } else {
        char numeral[4];
        *(int*)numeral = returnCode;
        Reply(caller, numeral, 4);
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
        command = requestBuf[0];
        symbol = requestBuf;
        symbol += 2;
        if (command == 'R') {
            // bwprintf(COM2, "Receiving registration return stringToNum(receiveBuffer, 10);%d, %s \r\n", caller, symbol);
            RegistrationPreamble(&NameTable, symbol, caller);
        } else if(command == 'W'){
            RetrievalPreamble(&NameTable, symbol, caller);
        } else {
            Reply(caller, "Bad operation", 13);
        }
    }
}

int getNsTid() {
    // if (nsTid == -1) {
    //     bwprintf(COM2, "PANIC: nameserver not up yet\r\n");
    //     for (;;) {}
    // }
    return nsTid;
}

int RegisterAs(const char *name){
    char buffer[100];
    char receiveBuffer[100];
    formatStrn(buffer, 100, "R %s", name);
    int result = Send(getNsTid(), buffer, strlen(buffer) + 1, receiveBuffer, 100);
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
    int result = Send(getNsTid(), buffer, strlen(buffer) + 1, receiveBuffer, 100);
    if(result>0){
        if(strcmp("Not found", receiveBuffer)) {
            return *(int*)receiveBuffer;
        } else {
            return -1;
        }
    } else {
        return -1;
    }
}
