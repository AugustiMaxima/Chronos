#include <stdlib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>
#include <track.h>
#include <conductor.h>
#include <tui.h>
#include <bwio.h>


//handles console command and dispatches tasks
//will be blocked on next input
void trackConsole(){
    //args:
    int RX;
    Conductor* conductor;
    TUIRenderState* prop;

    int value;
    int parent;

    Receive(&value, (char*)&RX, sizeof(RX));
    Reply(value, NULL, 0);
    Receive(&value, (char*)&conductor, sizeof(conductor));
    Reply(value, NULL, 0);
    Receive(&value, (char*)&prop, sizeof(prop));
    Reply(value, NULL, 0);

    //Important to reply at the end, otherwise this will never be cleaned up
    Receive(&parent, NULL, 0);

    char cmdBuffer[10];

    while(1){
        int status = GetLN(RX, 2, cmdBuffer, 10, 13, true);
        if(status<0){
            clearRXBuffer(RX, 2);
            //Consider showing an error message using TUI
            continue;
        }
        bwprintf(COM2, "%s", cmdBuffer);
        char* cmd = cmdBuffer;
        char* op1;
        int operand1;
        char* op2;
        int operand2;

        int op = 0;

        for(int i=0;cmdBuffer[i] && i<10;i++){
            if (cmdBuffer[i] == ' ' || cmdBuffer[i] == '\r'){
                cmdBuffer[i] = 0;
                if(op++ == 1){
                    op1 = cmdBuffer + i + 1;
                } else {
                    op2 = cmdBuffer + i + 1;
                    break;
                }
            }
        }

        if(!strcmp("rv", cmd)){
            operand1 = stringToNum(op1, 10);
            operand2 = stringToNum(op2, 10);
            setSpeedConductor(conductor, operand1, operand2);
        } else if(!strcmp("sw", cmd)){
            operand1 = stringToNum(op1, 10);
            switchConductor(conductor, operand1, op2[0]);
        } else if(!strcmp("rv", cmd)){
            operand1 = stringToNum(op1, 10);
            reverseConductor(conductor, operand1);
        } else if(!strcmp("q", cmd)){

        }
    }
}

int createTrackConsole(int RX, Conductor* conductor, TUIRenderState* prop){
    int console = Create(-1, trackConsole);
    Send(console, (const char*)&RX, sizeof(RX), NULL, 0);
    Send(console, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(console, (const char*)&prop, sizeof(prop), NULL, 0);
    return console;
}

//sets up and spawns every service related to train control
void k4_v2(){
    int ns = Create(-1, nameServer);
    int clk = Create(-1, clockServer);
    int rx1 = createRxServer(1);
    int tx1 = createTxServer(1);
    int rx2 = createRxServer(2);
    int tx2 = createTxServer(2);
    Conductor conductor;
    initializeConductor(&conductor, rx1, tx1, clk);
    TUIRenderState prop;
    int tui = createTUI(rx2, tx2, clk, &conductor, &prop);
    int console = createTrackConsole(rx2, &conductor, &prop);
    //Lock K4 to the console thread, so that K4 will not prematurely exit and destroy the shared state
    Send(console, NULL, 0);
}