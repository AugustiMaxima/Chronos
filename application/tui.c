#include <stdlib.h>
#include <syslib.h>
#include <uart.h>
#include <timer.h>
#include <uartServer.h>
#include <conductor.h>
#include <terminal.h>
#include <charay.h>
#include <tui.h>
#include <bwio.h>


//universal sanitization loop
int inputProcessing(char* source, int length, char* dest, int size){
    int di = 0, si = 0;
    for(si = 0; si < length && di < size; si++){
        if(source[si] == '\r'){
            dest[di++] = source[si];
            if(si+1<length && source[si+1] == '\n'){
                //do nothing            
            } else {
                dest[di++] = '\n';
            }
        } else if(source[si] == 8){
            //backspace
            di--;
        } else {
            dest[di++] = source[si];
        }
    }
    return di;
}

int drawInput(int RX2, int TX2, int index){
    const int inSize = 32;
    char buffer[inSize];
    int status = GleanUART(RX2, 2, index, buffer, inSize);
    if(status > 0){
        char processed[40];
        int length = inputProcessing(buffer, status, processed, 40);
        if(length < 0){
            TerminalOutput output;
            flush(&output);
            backSpace(&output, -length);
            PutCN(TX2, 2, output.compositePayload, output.length, true);
        } else {
            PutCN(TX2, 2, processed, length, true);
        }
    }
    return status + index;
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

    int index = 0;
    int event;

    //initialization message
    TerminalOutput formatter;
    flush(&formatter);
    clear(&formatter);
    setColor(&formatter, COLOR_GREEN);
    jumpCursor(&formatter, 0, 0);
    attachMessage(&formatter, "UI thread booted!\r\n");
    jumpCursor(&formatter, 7, 0);
    // bwprintf(COM2, "Ready: Formatter props: %d", formatter.length);
    PutCN(TX2, 2, formatter.compositePayload, formatter.length, true);

    while(1){
        //Practically any event happening related to the user ui or train track should result in this call
        event = AwaitMultipleEvent(&value, 2, INT_UART1, INT_UART2);
        index = drawInput(RX2, TX2, index);
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
