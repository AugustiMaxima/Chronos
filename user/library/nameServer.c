#include <map.h>
#include <charay.h>
#include <syslib.h>
#include <stdlib.h>

#define MAX_REQUEST 100
#define MAX_RESULT 100
#define MAX_OPERANDS 4

int Registration(Map* NameTable, char* symbol, int tId){
    return putMap(NameTable, stringHash(symbol), tId);
}

int Retrieve(Map* NameTable, char* symbol){
    return getMap(NameTable, stringHash(symbol));
}

void RegistrationPreamble(Map* NameTable, char** tokens, int caller){
    char* symbol, id;
    if(!tokens[2]){
        Reply(caller, "Missing symbols. Please check the API for nameserver", MAX_RESULT);
    } else {
        symbol = tokens[1];
        tokens[2][-1] = 0;
        if(!tokens[3]){
            Reply(caller, "Missing id. Please check the API for nameserver", MAX_RESULT);
        } else {
            id = tokens[2];
            tokens[3][-1] = 0;
            int tId = stringToNum(id, 10);
            int returnCode = Registration(&NameTable, symbol, tId);
            if(returnCode == 1)
                Reply(caller, "Registration successful.", MAX_RESULT);
            else if(returnCode == 0)
                Reply(caller, "Updated registration", MAX_RESULT);
            else
                Reply(caller, "Register failed due to insufficent resources", MAX_RESULT);
        }
    }
}


void RetrievalPreamble(Map* NameTable, char** tokens, int caller){
    char* symbol;
    if(!tokens[2]){
        Reply(caller, "Missing symbols. Please check the API for nameserver", MAX_RESULT);
    } else {
        symbol = tokens[1];
        tokens[2][-1] = 0;
        int returnCode = Retrieve(NameTable, symbol);
        if(!returnCode){
            Reply(caller, "Registration not found", MAX_RESULT);
        } else {
            char numeral[MAX_RESULT];
            noneZeroIntString(numeral, returnCode, 10);
            Reply(caller, numeral, MAX_RESULT);
        }
    }
}

void nameServer(){
    Map NameTable;
    initializeMap(&NameTable);
    int caller;
    char* requestBuf[MAX_REQUEST];
    char* tokens[MAX_OPERANDS];
    char* command;
    while(Receive(&caller, requestBuf, MAX_REQUEST)){
        int i;
        for(i=0;i<MAX_OPERANDS;i++){
            tokens[i] = NULL;
        }
        split(tokens, 3, requestBuf, ' ');
        if(!tokens[1]){
            Reply(caller, "Missing commands. Please check the API for nameserver", MAX_RESULT);
            continue;
        } else {
            command = tokens[0];
            tokens[1][-1] = 0;
        }
        if(strcmp(command, "Register") == 0){
            RegistrationPreamble(&NameTable, tokens, caller);
        } else if(strcmp(command, "Retrieve")){
            RetrievalPreamble(&NameTable, tokens, caller);
        }
    }
}

int RegisterAs(const char *name){
    int tid = MyTid();
    
}

int WhoIs(const char *name);