#include <stdlib.h>
#include <syslib.h>
#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>
#include <track.h>
#include <conductor.h>
#include <pathFinder.h>
#include <tui.h>
#include <charay.h>
#include <bwio.h>



void processUserRequestV2(char* command, Conductor* conductor, TUIRenderState* prop){
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
            } else if(op++==1) {
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
    } else if(!strcmp("go", cmd)) {
        //note to self, refactor this
        int source = nameAttribution(op1, conductor->trackNodes);
        int dest = nameAttribution(op2, conductor->trackNodes);
        PATH path;
        computePath(conductor->trackNodes, &path, source, dest);
        if(path.cost[dest] == -1){
            return;
        }
        TRACKEVENT events[64];
        parsePath(conductor->trackNodes, &path, events, 64, dest);
        //time to use the track event

        int eventProcessed = 0; 
        //figure out the stoppin condition later
        int sensor = -1;
        int distance;
        for(int i=0; events[i].type != END; i++){
            if(events[i].type == BRANCH){
                Delay(conductor->CLK, 5);
                switchConductor(conductor, events[i].id, (events[i].auxiliary == 1 ? 'C' : 'S'));
            } else if(events[i].type == SENSOR){
                sensor = events[i].id;
            }
        }
        setSpeedConductor(conductor, 1, 14);

        distance = path.cost[dest] - path.cost[conductor->index.sensorToNode[sensor]];

        while(true){
            getSensorData(conductor);            
            if(conductor->sensor[sensor]){
                break;
            }



        }
        setSpeedConductor(conductor, 1, 0);

    } else if(!strcmp("q", cmd)){
        Shutdown();
    } else {
        //do something warning text here
    }
}


//handles console command and dispatches tasks
//will be blocked on next input
void trackController(){
}

int createTrackController(int RX, int CLK, Conductor* conductor, TUIRenderState* prop){
    int console = Create(-1, trackController);
    Send(console, (const char*)&RX, sizeof(RX), NULL, 0);
    Send(console, (const char*)&CLK, sizeof(CLK), NULL, 0);
    Send(console, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(console, (const char*)&prop, sizeof(prop), NULL, 0);
    return console;
}

//sets up and spawns every service related to train control
void TC1(){
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
    int console = createTrackController(rx2, clk, &conductor, &prop);
    //Lock K4 to the console thread, so that K4 will not prematurely exit and destroy the shared state
    Send(console, NULL, 0, NULL, 0);
}