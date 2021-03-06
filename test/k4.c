#include <stdlib.h>
#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>
#include <track.h>
#include <conductor.h>
#include <tui.h>
#include <charay.h>
#include <bwio.h>


void processUserRequest(char* command, Conductor* conductor, TUIRenderState* prop){
    char* cmd = command;
    char* op1;
    int operand1;
    char* op2;
    int operand2;

    int op = 0;

    for(int i=0;command[i] && i<16;i++){
        if (command[i] == ' ' || command[i] == '\r'){
            command[i] = 0;
            if(op++ == 0){
                op1 = command + i + 1;
            } else {
                op2 = command + i + 1;
                break;
            }
        }
    }

    if(!strcmp("tr", cmd)){
        operand1 = stringToNum(op1, 10);
        operand2 = stringToNum(op2, 10);
        setSpeedConductor(conductor, operand1, operand2);
    } else if(!strcmp("sw", cmd)){
        operand1 = stringToNum(op1, 10);
        switchConductor(conductor, operand1, op2[0]);
        prop->switchUpdate = true;
    } else if(!strcmp("rv", cmd)){
        operand1 = stringToNum(op1, 10);
        reverseConductor(conductor, operand1);
    } else if(!strcmp("q", cmd)){
        Shutdown();
    } else {
        //do something warning text here
    }
}


//handles console command and dispatches tasks
//will be blocked on next input
void trackConsole(){
    //args:
    int RX;
    int CLK;
    Conductor* conductor;
    TUIRenderState* prop;

    int value;
    int parent;

    Receive(&value, (char*)&RX, sizeof(RX));
    Reply(value, NULL, 0);
    Receive(&value, (char*)&CLK, sizeof(CLK));
    Reply(value, NULL, 0);
    Receive(&value, (char*)&conductor, sizeof(conductor));
    Reply(value, NULL, 0);
    Receive(&value, (char*)&prop, sizeof(prop));
    Reply(value, NULL, 0);

    //Important to reply at the end, otherwise this will never be cleaned up
    Receive(&parent, NULL, 0);

    char cmdBuffer[16];
    int bufDex;
    char command[16];
    int length;
    //used to support arrow keys
    //we will leave this for another day as it introduces significant complexities
    char postfix[10];
    while(1){
        int status = GetLN(RX, 2, cmdBuffer, 16, 13, false);
        if(status == -2){
            clearRXBuffer(RX, 2);
            //Consider showing an error message using TUI
            Delay(CLK, 10);
            continue;
        } else if(status == -1){
            //not enough key strokes
            //opportunity for sensor update
            Delay(CLK, 10);
            getSensorData(conductor);
            prop->sensorUpdate = true;
            Delay(CLK, 10);
            continue;
        } else {
            //processing buffered data with backspace and cursors into correct command
            length = 0;
	        int bufDex = 0;
	        //TODO: Fix the backspace
	        //Works for one stroke, but freak out over 2 strokes
            for(; bufDex < status; bufDex++){
                //backspace
                if(cmdBuffer[bufDex] == 8){
                    if(length>0){
                        length--;
                    }
                } else {
                    command[length++] = cmdBuffer[bufDex];
                }
            }
            processUserRequest(command, conductor, prop);
        }
    }
}

int createTrackConsole(int RX, int CLK, Conductor* conductor, TUIRenderState* prop){
    int console = Create(-1, trackConsole);
    Send(console, (const char*)&RX, sizeof(RX), NULL, 0);
    Send(console, (const char*)&CLK, sizeof(CLK), NULL, 0);
    Send(console, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(console, (const char*)&prop, sizeof(prop), NULL, 0);
    return console;
}

//sets up and spawns every service related to train control
void k4(){
    bwprintf(COM2, "Constructing child threads...\r\n");
    int ns = Create(-1, nameServer);
    int clk = Create(-1, clockServer);
    int rx1 = createRxServer(1);
    int tx1 = createTxServer(1);
    int rx2 = createRxServer(2);
    int tx2 = createTxServer(2);
    Conductor conductor;
    //Takes either A or B
    bwprintf(COM2, "Please enter the track name you are running this demo on: (A/B):    ");
    char track;
    //will block until you enter something here
    GetCN(rx2, 2, &track, 1, true);
    if(track == 'A' || track == 'a')
        track = 1;
    else if(track == 'B' || track == 'b')
        track = 2;
    else{
        //TODO: make q work with this
        bwprintf(COM2, "\r\nInvalid track! Ok ciao!\r\n");
        Shutdown();
    }
    bwprintf(COM2, "\r\nInitializing track\r\n");
    initializeConductor(&conductor, rx1, tx1, clk, track);
    bwprintf(COM2, "Track finished, spinning up controller and ui thread\r\n");
    TUIRenderState prop;
    int tui = createTUI(rx2, tx2, clk, &conductor, &prop);
    int console = createTrackConsole(rx2, clk, &conductor, &prop);
    //Lock K4 to the console thread, so that K4 will not prematurely exit and destroy the shared state
    Send(console, NULL, 0, NULL, 0);
}
