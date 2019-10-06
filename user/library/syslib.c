#include <syscode.h>
#include <syslib.h>
#include <bwio.h>
#include <scheduler.h>

#define MAX_ARGS 10

static inline __attribute__((always_inline)) int save_user_context() {
    asm("STMFD SP!, {R0-R12, R14-R15}");
    asm("MRS R2, CPSR");
    asm("STMFD SP!, {R2}");
}

int Create(int priority, void (*function)()){
    save_user_context();
    //argument stuffing now
    asm("STMFD SP!, {R0-R1}");

    asm("SWI " TOSTRING(CREATE_CODE) ::: "r0");
    int ret;
    asm("MOV %0, R0" : "=r" (ret));
    return ret;
}

int MyTid(){
    save_user_context();
    asm("SWI " TOSTRING(MYTID_CODE) ::: "r0");
    int ret;
    asm("MOV %0, R0" : "=r" (ret));
    return ret;
}

int MyParentTid(){
    save_user_context();
    asm("SWI " TOSTRING(MYPARENTTID_CODE) ::: "r0");
    int ret;
    asm("MOV %0, R0" : "=r" (ret));
    return ret;
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
    va_end(varags);
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
