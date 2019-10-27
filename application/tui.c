#include <stdlib.h>
#include <syslib.h>
#include <uart.h>
#include <timer.h>
#include <uartServer.h>
#include <conductor.h>
#include <terminal.h>
#include <tui.h>


void drawInput(int TX2, char* buffer, int length){
    PutCN(TX2, 2, buffer, length, true);
}




//Needs the value of TX2 and RX2
//Low prioirity should be fine for this task
//Needs more handle on the track state, working on getting this now
void tuiThread(){
    //args
    int RX2;
    int TX2;
    int CLK;
    Conductor* conductor;
    TUIRenderState* prop;

    //if state is NULL, will rerender everything everytime, very costly

    int value;
    Receive(&value, (char*)(&RX2), sizeof(RX2));
    Reply(value, NULL, 0);
    Receive(&value, (char*)(&TX2), sizeof(TX2));
    Reply(value, NULL, 0);

    //In case we are interested in implementing exponential backoff
    Receive(&value, (char*)(&CLK), sizeof(CLK));
    Reply(value, NULL, 0);

    //State sharing for track status
    Receive(&value, (char*)&conductor, sizeof(conductor));
    Reply(value, NULL, 0);
    Receive(&value, (char*)&prop, sizeof(prop));
    Reply(value, NULL, 0);

    char buffer[256];
    int index = 0;
    int event;

    while(1){
        //Practically any event happening related to the user ui or train track should result in this call
        event = AwaitMultipleEvent(&value, 2, INT_UART1, INT_UART2);
        int status = GleanUART(RX2, 2, index, buffer, 256);
        if(status > 0){
            index+=status;
            drawInput(TX2, buffer, status);
        }
    }
}

int createTUI(int RX, int TX, int CLK, Conductor* conductor, TUIRenderState* prop){
    int tui = Create(-1, tuiThread);
    Send(tui, (const char*)&RX, sizeof(RX), NULL, 0);
    Send(tui, (const char*)&TX, sizeof(TX), NULL, 0);
    Send(tui, (const char*)&CLK, sizeof(CLK), NULL, 0);
    Send(tui, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(tui, (const char*)&prop, sizeof(prop), NULL, 0);
    return tui;
}