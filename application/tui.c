#include <stdlib.h>
#include <syslib.h>
#include <uart.h>
#include <timer.h>
#include <uartServer.h>
#include <conductor.h>
#include <terminal.h>
#include <uiHelper.h>
#include <charay.h>
#include <tui.h>
#include <bwio.h>

static const char shellMsg[16] = "ChronoShell: ";

int sensorPn = 0;

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
                for(int i = 0; shellMsg[i] && di<size; i++)
                    dest[di++] = shellMsg[i];
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
        char processed[64];
        int length = inputProcessing(buffer, status, processed, 64);
        if(length < 0){
            TerminalOutput output;
            flush(&output);
            backSpace(&output, -length);
            PutCN(TX2, 2, output.compositePayload, output.length, true);
        } else {
            PutCN(TX2, 2, processed, length, true);
        }
	return status + index;
    } else {
	return index;
    }
}

void renderUtilizationRate(int TX2, int rate){
    TerminalOutput output;
    uiUtilizationRate(&output, rate);
    PutCN(TX2, 2, output.compositePayload, output.length, true);
}

void renderTime(int TX2, int time){
    TerminalOutput output;
    uiTimeStamp(&output, time);
    PutCN(TX2, 2, output.compositePayload, output.length, true);
}

void renderSensor(int TX2, char sensors[SENSOR_COUNT], Conductor* conductor){
    int newSensorList[8];
    int newSensorCount = 0;
    for(int i=0; i<SENSOR_COUNT; i++){
        if(conductor->sensor[i] != sensors[i]){
            sensors[i] = conductor->sensor[i];
            if(newSensorCount < 8 && sensors[i])
                newSensorList[newSensorCount++] = i;
        }
    }
    TerminalOutput output;
    flush(&output);
    saveCursor(&output);
    for(int i=0; i<newSensorCount; i++){
        sensorPn %= 8;
        jumpCursor(&output, 3, 8*sensorPn + 2);
        attachMessage(&output, "    ");
        jumpCursor(&output, 3, 8*sensorPn++);
        attachMessage(&output, conductor->trackNodes[(int)conductor->index.sensorToNode[newSensorList[i]]].name);
    }
    if(sensorPn){
        jumpCursor(&output, 4, 8*(sensorPn - 1));
        if(sensorPn == 1)
            deleteLine(&output);
        attachMessage(&output, "=>");
        restoreCursor(&output);
    }
    PutCN(TX2, 2, output.compositePayload, output.length, true);
}

void renderSwitches(int TX2, char* switches, Conductor* conductor){
    TerminalOutput output;
    flush(&output);
    saveCursor(&output);
    char buff[2];
    buff[1] = 0;
    for(int i=0; i<SWITCH_COUNT; i++){
        if(switches[i] != conductor->switches[i]){
            jumpCursor(&output, 5, i*3);
            switches[i] = conductor->switches[i];
            buff[0] = switches[i];
            attachMessage(&output, buff);
        }
    }
    restoreCursor(&output);
    PutCN(TX2, 2, output.compositePayload, output.length, true);
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
    KernelMetaData* metadata = getKernelMetaData();

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

    //Some state data for rendering branches and sensors
    char switches[SWITCH_COUNT];
    char sensor[SENSOR_COUNT];

    //ignores any extra keystrokes generated during kernel initialization
    index = GleanUART(RX2, 2, index, sensor, SENSOR_COUNT);

    PutCN(TX2, 2, "UI Thread initializing\r\n", strlen("UI Thread initializing\r\n"), true);
    
    for(int i=0; i<SWITCH_COUNT; i++){
        switches[i] = conductor->switches[i];
    }
    for(int i=0; i<SENSOR_COUNT; i++){
        sensor[i] = conductor->sensor[i];
    }


    //initialization message
    TerminalOutput formatter;
    flush(&formatter);
    clear(&formatter);
    setWindowBoundary(&formatter, 16, 48);
    jumpCursor(&formatter, 15, 0);
    PutCN(TX2, 2, formatter.compositePayload, formatter.length, true);
    PutCN(TX2, 2, "________________________________________________________________\r\n", 66, true);
    PutCN(TX2, 2, shellMsg, strlen(shellMsg), true);

    int time = 0;

    while(1){
        //Practically any event happening related to the user ui or train track should result in this call
        event = AwaitMultipleEvent(&value, 3, TC1UI_DEV_ID, INT_UART1, INT_UART2);
        index = drawInput(RX2, TX2, index);
        if(event == TC1UI_DEV_ID){
            if(++time%10 == 0)
                prop->timeUpdate = true;
        }

        if(prop->timeUpdate){
            prop->timeUpdate = false;
            renderTime(TX2, time);
        }

        if(time%100 == 0){
            renderUtilizationRate(TX2, metadata->utilizationRate);
        }

        if(prop->sensorUpdate){
            renderSensor(TX2, sensor, conductor);
            prop->sensorUpdate = false;
        }

        if(prop->switchUpdate){
            renderSwitches(TX2, switches, conductor);
            prop->switchUpdate = false;
        }

        if(!prop->message.read){
            //Render warning message
            prop->message.read = true;
        }
    }
}

int createTUI(int RX, int TX, int CLK, Conductor* conductor, TUIRenderState* prop){
    int tui = Create(1, tuiThread);
    Send(tui, (const char*)&RX, sizeof(RX), NULL, 0);
    Send(tui, (const char*)&TX, sizeof(TX), NULL, 0);
    Send(tui, (const char*)&CLK, sizeof(CLK), NULL, 0);
    Send(tui, (const char*)&conductor, sizeof(conductor), NULL, 0);
    Send(tui, (const char*)&prop, sizeof(prop), NULL, 0);
    return tui;
}