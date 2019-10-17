#include <bwio.h>
#include <ts7200.h>
#include <sendReceiveReply.h>
#include <interrupt.h>
#include <nameServer.h>
#include <clockServer.h>
#include <queue.h>

#define MSG_MAX 24

void printCharToCom2(char c) {
    int serverTid = WhoIs("com2TxS");
    char replyBuf[10];

    char sendReq[2];
    sendReq[0] = 'S';
    sendReq[1] = c;
    Send(serverTid, sendReq, 2, replyBuf, 10);
}

void printCharToCom1(char c) {
    int serverTid = WhoIs("com1TxS");
    char replyBuf[10];

    char sendReq[2];
    sendReq[0] = 'S';
    sendReq[1] = c;
    Send(serverTid, sendReq, 2, replyBuf, 10);
}

void com2TxServer() {
    RegisterAs("com2TxS");

    char requestBuf[MSG_MAX];
    char command;
    int caller;

    int notifierTid;

    Queue com1Queue;
    initializeQueue(&com1Queue);

    int txNotifierReady = 0;

    while (1) {
        Receive(&caller, requestBuf, MSG_MAX);
        if (requestBuf[0] == 'R'/*eady*/) {
            txNotifierReady = 1;
            notifierTid = caller;
        } else if (requestBuf[0] == 'S'/*end*/) {
            push(&com1Queue, (void*)requestBuf[1]);
            Reply(caller, "ok", strlen("ok"));
        } else {
            setEnabledDevices(0x0, 0x0);
            bwprintf(COM2, "PANIC: Unknown IPC message\r\n");
        }
        if (txNotifierReady && ringFill(&com1Queue)) {
            char c = (char) pop(&com1Queue);
            Reply(notifierTid, &c, 1);
            txNotifierReady = 0;
        }
    }
}

void com2TxNotifier() {
    RegisterAs("com2TxN");

    int serverTid = WhoIs("com2TxS");

    int volatile * flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART2_BASE + UART_DATA_OFFSET );

    while (1) {
        AwaitEvent(UART2TX_DEV_ID);
        char c;
        Send(serverTid, "R", strlen("R"), &c, 1);
        if (*flags & TXFF_MASK) {
            setEnabledDevices(0x0, 0x0);
            bwprintf(COM2, "PANIC: Return from UART interrupt but TX FIFO is full\r\n");
            for (;;) {}
        }
        *data = c;
    }

}

int requestingSensorData;

void sensorRequestor() {
    int serverTid = WhoIs("com1TxS");

    char replyBuf[10];

    char sendReq[2];
    sendReq[0] = 'S';
    sendReq[1] = 0x85;

    requestingSensorData = 1;
    while (1) {
        if (requestingSensorData) {
            Send(serverTid, sendReq, 2, replyBuf, 10);
        }
        Delay(WhoIs("cs"), 100); // wait 1s
    }
}

void com1TxServer() {
    RegisterAs("com1TxS");

    char requestBuf[MSG_MAX];
    char command;
    int caller;

    int notifierTid;

    Queue com1Queue;
    initializeQueue(&com1Queue);

    int txNotifierReady = 0;

    while (1) {
        Receive(&caller, requestBuf, MSG_MAX);
        if (requestBuf[0] == 'R'/*eady*/) {
            txNotifierReady = 1;
            notifierTid = caller;
        } else if (requestBuf[0] == 'S'/*end*/) {
            push(&com1Queue, (void*)requestBuf[1]);
            Reply(caller, "ok", strlen("ok"));
        } else {
            setEnabledDevices(0x0, 0x0);
            bwprintf(COM2, "PANIC: Unknown IPC message\r\n");
        }

        if (txNotifierReady && ringFill(&com1Queue)) {
            char c = (char) pop(&com1Queue);
            Reply(notifierTid, &c, 1);
            txNotifierReady = 0;
        }
    }
}

void com1TxNotifier() {
    RegisterAs("com1TxN");

    int serverTid = WhoIs("com1TxS");

    int volatile * flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART1_BASE + UART_DATA_OFFSET );

    while (1) {
        AwaitEvent(UART1TX_DEV_ID);
        char c;
        Send(serverTid, "R", strlen("R"), &c, 1);
        // todo: why is neturn code of the above == 5
        if (*flags & TXFF_MASK) {
            setEnabledDevices(0x0, 0x0);
            bwprintf(COM2, "PANIC: Return from UART interrupt but TX FIFO is full\r\n");
            for (;;) {}
        }
        *data = c;
    }

}

void printSensorReport(char* com1_input) {
    int sensors_state[80];
    int i, j;
    for (i=0;i<10;i++) {
        for (j=0;j<8;j++) {
            int sensor_id = i*8 + j;
            sensors_state[sensor_id] = com1_input[i] & (1 << j);
        }
    }
    for (i=0; i<40; i++) {
        int left = sensors_state[2*i];
        int right = sensors_state[2*i + 1];

        if (left && right) {
            printCharToCom2('x');
        } else if (left) {
            printCharToCom2('<');
        } else if (right) {
            printCharToCom2('>');
        } else {
            printCharToCom2('_');
        }
    }
    printCharToCom2('\r');
    printCharToCom2('\n');
}

void switchesTo(char pos) {
    requestingSensorData = 0;
    char i;
    for (i=1; i<=18; i++) {
        printCharToCom1(pos);
        printCharToCom1(i);
        Delay(WhoIs("cs"), 35); // wait 350ms
    }
    for (i=0x9A; i<=0x9C; i++) {
        printCharToCom1(pos);
        printCharToCom1(i);
        Delay(WhoIs("cs"), 35); // wait 350ms
    }
    for (i=0x99; i<=0x99; i++) {
        printCharToCom1(pos);
        printCharToCom1(i);
        Delay(WhoIs("cs"), 35); // wait 350ms
    }
    // Sei vorsichtig!!!!!!111
    // Mach die solenoiden auf
    printCharToCom1(0x20);
    requestingSensorData = 1;
}

void switchStraight() {
    switchesTo(0x21);
}

void switchCurved() {
    switchesTo(0x22);
}

void handleUserInput(char* user_input) {
    if (0 == strcmp(user_input, "as")) {
        Create(10, switchStraight);
    } else if (0 == strcmp(user_input, "ac")) {
        Create(10, switchCurved);
    }
}

void com2RxNotifier() {
    int volatile * flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART2_BASE + UART_DATA_OFFSET );

    char user_input[10];
    int user_input_end = 0;

    for (;;) {
        AwaitEvent(UART2RX_DEV_ID);
        while (!(*flags & RXFE_MASK)) {
            char ch = *data;
            if (ch == '\r') {
                printCharToCom2('\r');
                printCharToCom2('\n');
                user_input[user_input_end++] = 0;
                handleUserInput(user_input);
                user_input_end = 0;
            } else {
                printCharToCom2(ch);
                user_input[user_input_end++] = ch;
            }
        }
    }
}

void com1RxNotifier() {
    int volatile * flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
    int volatile * data = (int *)( UART1_BASE + UART_DATA_OFFSET );

    char com1_input[10];
    int com1_input_end = 0;

    for (;;) {
        AwaitEvent(UART1RX_DEV_ID);
        while (!(*flags & RXFE_MASK)) {
            // bwprintf(COM2, "COM1 received %x\r\n", *data);
            com1_input[com1_input_end++] = *data;
        }

        if (com1_input_end == 10) {
            printSensorReport(com1_input);
            com1_input_end = 0;
        }

    }
}

void k4_main() {
    Create(-1, nameServer);
    Create(-1, clockServer);

    Create(1, com1RxNotifier);
    Create(2, com1TxServer);
    Create(3, com1TxNotifier);
    Create(4, sensorRequestor);

    Create(1, com2RxNotifier);
    Create(2, com2TxServer);
    Create(3, com2TxNotifier);
}