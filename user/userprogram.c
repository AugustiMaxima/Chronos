#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>
#include <stdlib.h>
#include <map.h>

void receiver() {
    int who;
    char buf[100];
    int ret = Receive(&who, buf, 40);
    bwprintf(COM2, "[receiver]\tReceive(=%d, =%s, 40)=%d\r\n", who, buf, ret);
    Reply(who, "Who the fuck are ye?", 40);
}

void sender() {
    char buf[100];
    int ret = Send(2, "hello", 40, buf, 40);
    bwprintf(COM2, "[sender]\tSend(2, hello, 40, =%s, 40)=%d\r\n", buf, ret);
}

void user_main() {
    bwprintf(COM2, "[user_main]\tCreate(-1, receiver)=%d\r\n", Create(-1, receiver));
    bwprintf(COM2, "[user_main]\tCreate(-1, sender)=%d\r\n", Create(-1, sender));
}

void shitTest(){
    Destroy();
}

void FireStrike(){
    int i=0;
    int exp = 0;
    for(i = 0; i<10000; i++){
        int id = Create(2, shitTest);
        if(id>0){
            exp++;
        }
    }
    bwprintf(COM2, "Created %d tasks\r\n", exp);
}

