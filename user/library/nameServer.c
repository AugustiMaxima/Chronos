#include <map.h>
#include <charay.h>
#include <syslib.h>
#include <stdlib.h>
#include <bwio.h>

#define MAX_REQUEST 100
#define MAX_RESULT 100

int Registration(Map* NameTable, char* symbol, int tId){
    bwprintf(COM2, "Registering %d with %s\r\n", tId, symbol);
    return putMap(NameTable, alphaNumericHash(symbol), tId);
}

int Retrieve(Map* NameTable, char* symbol){
    int stuff = getMap(NameTable, alphaNumericHash(symbol));
    bwprintf(COM2, "Getting %s, we think it's %d\r\n", symbol, stuff);
    return stuff;
}

void RegistrationPreamble(Map* NameTable, char* symbol, int caller){
    int returnCode = Registration(&NameTable, symbol, caller);
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

void nameServer(){
    Map NameTable;
    initializeMap(&NameTable);
    int caller;
    char* requestBuf[MAX_REQUEST];
    char command;
    char* symbol;
    while(Receive(&caller, requestBuf, MAX_REQUEST)){
        command = requestBuf[0];
        symbol = requestBuf;
        symbol++;
        symbol++;
        bwprintf(COM2, "symbol=%s\r\n", symbol);
        if(command == 'R'){
            RegistrationPreamble(&NameTable, symbol, caller);
        } else if(command == 'W'){
            RetrievalPreamble(&NameTable, symbol, caller);
        } else {
            Reply(caller, "Bad operation", MAX_RESULT);
        }
    }
}

int RegisterAs(const char *name){
    char* buffer[100];
    char* receiveBuffer[100];
    formatStrn(buffer, 100, "R %s", name);
    bwprintf(COM2, "%s\r\n", buffer);
    int result = Send(1, buffer, 100, receiveBuffer, 100);
    if(result>0){
        return 0;
    } else {
        return -1;
    }
}

int WhoIs(const char *name){
    char* buffer[100];
    char* receiveBuffer[100];
    formatStrn(buffer, 100, "W %s", name);
    int result = Send(1, buffer, 100, receiveBuffer, 100);
    if(result>0){
        if(strlcmp("Registration not found", receiveBuffer))
            return stringToNum(receiveBuffer, 10);
        else
            return 0;
    } else {
        return -1;
    }
}