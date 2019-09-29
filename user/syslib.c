#include <syscode.h>
#include <syslib.h>
#include <bwio.h>
#include <scheduler.h>

#define MAX_ARGS 10

static inline __attribute__((always_inline)) int save_user_context() {
    asm("SUB SP, SP, #64");
    asm("STR R0, [SP]");
    asm("STR R1, [SP, #4]");
    asm("STR R2, [SP, #8]");
    asm("STR R3, [SP, #12]");
    asm("STR R4, [SP, #16]");
    asm("STR R5, [SP, #20]");
    asm("STR R6, [SP, #24]");
    asm("STR R7, [SP, #28]");
    asm("STR R8, [SP, #32]");
    asm("STR R9, [SP, #36]");
    asm("STR R10, [SP, #40]");
    asm("STR R11, [SP, #44]");
    asm("STR R12, [SP, #48]");
    asm("STR R13, [SP, #52]");
    asm("STR R14, [SP, #56]");
    asm("SUB SP, SP, #4");
    asm("MRS R2, CPSR");
    asm("STR R2, [SP]");
}

int Create(int priority, void (*function)()){
    save_user_context();
    //argument stuffing now
    asm("SUB SP, SP, #8");
    asm("STR R0, [SP]");
    asm("STR R1, [SP, #4]");

    asm("SWI " TOSTRING(CREATE_CODE));
}

int MyTid(){
    save_user_context();
    asm("SWI " TOSTRING(MYTID_CODE));
}

int MyParentTid(){
    save_user_context();
    asm("SWI " TOSTRING(MYPARENTTID_CODE));
}

void Yield(){
    save_user_context();
    asm("SWI " TOSTRING(YIELD_CODE));
}

void Exit(){
    save_user_context();
    asm("SWI " TOSTRING(EXIT_CODE));
}

void Destroy() {
    save_user_context();
    asm("SWI " TOSTRING(DESTROY_CODE));
}

typedef struct usrargs{
    int args[MAX_ARGS];
} Arguments;

void variadicStore(Arguments* argv, int argc, ...){
    va_list varags;
    va_start(varags, argc);
    int i;
    for(i=0;i<argc;i++){
        argv->args[i] = va_arg(varags, int);
    }
}

int Send(int tid, const char *msg, int msglen, char *reply, int replylen) {
    Arguments argument;
    variadicStore(&argument, 5, tid, msg, msglen, reply, replylen);
    save_user_context();
    asm("SUB SP, SP, #4");
    asm("STR %0, [SP]"::"r"(argument.args));
    asm("SWI " TOSTRING(SEND_CODE));
}

int Receive(int *tid, char *msg, int msglen) {
    Arguments argument;
    variadicStore(&argument, 3, tid, msg, msglen);
    save_user_context();

    asm("SUB SP, SP, #4");
    asm("STR %0, [SP]"::"r"(argument.args));
    asm("SWI " TOSTRING(RECEIVE_CODE));

    //Pro gamer move:
    //pushes the sender into tid, stored at R2
    asm("STR R1, [R2]");
}

int Reply( int tid, const char *reply, int replylen ) {
    Arguments argument;
    variadicStore(&argument, 3, tid, reply, replylen);
    save_user_context();
    asm("SUB SP, SP, #4");
    asm("STR %0, [SP]"::"r"(argument.args));
    asm("SWI " TOSTRING(REPLY_CODE));
}
