#include <bwio.h>
#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <charay.h>

// void idle() {
//     for (;;) {
//     }
// }

void delayer(int delayTime, int numDelays) {
    int csTid = WhoIs("cs");
    for (int i=0; i<numDelays; i++) {
        bwprintf(COM2, "time=%d  \ttId=%d\r\n", Time(csTid), MyTid());
        Delay(csTid, delayTime);
    }
}

void delayer1() {
    delayer(10, 20);
    bwprintf(COM2, "time=%d  \ttId=%d \t done\r\n", Time(WhoIs("cs")), MyTid());
}

void delayer2() {
    delayer(23, 9);
    bwprintf(COM2, "time=%d  \ttId=%d \t done\r\n", Time(WhoIs("cs")), MyTid());
}

void delayer3() {
    delayer(33, 6);
    bwprintf(COM2, "time=%d  \ttId=%d \t done\r\n", Time(WhoIs("cs")), MyTid());
}

void delayer4() {
    delayer(71, 3);
    bwprintf(COM2, "time=%d  \ttId=%d \t done\r\n", Time(WhoIs("cs")), MyTid());
}

void k3_main() {
    Create(-1, nameServer);
    Create(-1, clockServer);
    Create(3, delayer1);
    Create(4, delayer2);
    Create(5, delayer3);
    Create(6, delayer4);
}
