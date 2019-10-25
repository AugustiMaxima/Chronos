#include <syslib.h>
#include <timer.h>
#include <string.h>
#include <interrupt.h>
#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>
#include <track.h>
#include <deviceTests.h>
#include <bwio.h>
#include <stdlib.h>
#include <terminal.h>

void windows(){
    int val;
    int device;
    initializeTimer(2, 508000, 5080, 1); // 10ms
    enableDeviceInterrupt(TC2UI_DEV_ID);
    while(1){
	    device = AwaitMultipleEvent(&val, 2, TC1UI_DEV_ID, TC2UI_DEV_ID);		
	    bwprintf(COM2, "Timer %d with value %d\r\n", device, val);
    }
}


void uartServerTest(){
    int buf = 1;
    Create(-1, nameServer);
    int RX1 = Create(-1, rxServer);
    Send(RX1, (const char*)&buf, sizeof(buf), NULL, 0);
    int TX1 = Create(-1, txServer);
    char buf2[5];
    buf2[4] = 0;
    while(1){
	GetCN(RX1, 1, buf2, 10, true);
	bwprintf(COM2, "%s\r\n", buf2);
    }
    
}

void control(){
    Create(-1, nameServer);
    int RX1 = Create(-1, rxServer);
    int RX2 = Create(-1, rxServer);
    int TX1 = Create(-1, txServer);
    int TX2 = Create(-1, txServer);
    int config = 1;
    Send(RX1, (const char*)&config, sizeof(config), NULL, 0);
    Send(TX1, (const char*)&config, sizeof(config), NULL, 0);
    config++;
    Send(RX2, (const char*)&config, sizeof(config), NULL, 0);
    Send(TX2, (const char*)&config, sizeof(config), NULL, 0);
    int clock = Create(-1, clockServer);
    
    bool sensorBanks[80];
    char sensorCount[83];
    
    while(1){
        sendSensorRequest(TX1);
        Delay(clock, 100);
        getSensorReading(RX1, sensorBanks);
        for(int i=0;i<80;i++){
            if(sensorBanks[i]){
                sensorCount[i] = 'X';
            }else{
                sensorCount[i] = '_';
            }
        }
        sensorCount[80] = '\r';
        sensorCount[81] = '\n';
	sensorCount[82] = 0;
        //PutCN(TX2, 2, "Printing sensors:\r\n", strlen("Printing sensors:\r\n"), true);
        PutCN(TX2, 2, sensorCount, 82, true);
        //bwprintf(COM2, "Printing sensors:\r\n%s\r\n", sensorCount);
        Delay(clock, 100);
    }
}


#include <nameServer.h>
#include <clockServer.h>
#include <uartServer.h>
#include <track.h>
#include <deviceTests.h>
#include <bwio.h>
#include <stdlib.h>
#include <terminal.h>



void k4_v2(){
    Create(-1, nameServer);
    int RX1 = Create(-1, rxServer);
    int RX2 = Create(-1, rxServer);
    int TX1 = Create(-1, txServer);
    int TX2 = Create(-1, txServer);

    int config = 1;
    Send(RX1, (const char*)&config, sizeof(config), NULL, 0);
    Send(TX1, (const char*)&config, sizeof(config), NULL, 0);
    config++;
    Send(RX2, (const char*)&config, sizeof(config), NULL, 0);
    Send(TX2, (const char*)&config, sizeof(config), NULL, 0);

    int clock = Create(-1, clockServer);

    int ui_index = 0;

    char cmdBuffer[10];

    char displayBuffer[64];

    TerminalOutput output;

    while(1){
        int addition = GleanUART(RX2, 2, ui_index, displayBuffer, 64);
        //flush(&output);
        //saveCursor(&output);
        //PutCN(TX2, 2, output.compositePayload, output.length, true);
        PutCN(TX2, 2, displayBuffer, addition, true);
        ui_index += addition;
        //flush(&output);
        //restoreCursor(&output);
        //PutCN(TX2, 2, output.compositePayload, output.length, true);
        int status = GetLN(RX2, 2, cmdBuffer, 10, 13, true);
        if(status>0){
	    for(int i=0;cmdBuffer[i] != 13; i++){
		bwprintf(COM2, "%c %d\r\n", cmdBuffer[i], cmdBuffer[i]);
	    }
	} else {
	    GetCN(RX2, 2, cmdBuffer, 10, true);
	    //clean up by draining the bad data
	    continue;
	}

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
            engineSpeed(TX1, operand1, operand2);
        } else if(!strcmp("sw", cmd)){
            operand1 = stringToNum(op1, 10);
            branch(TX1, operand1, *op2);
            Delay(clock, 20);
            turnOut(TX1);
        } else if(!strcmp("rv", cmd)){
            operand1 = stringToNum(op1, 10);
            engineSpeed(TX1, operand1, 0);
            Delay(clock, 40);
            engineSpeed(TX1, operand1, 15);
            Delay(clock, 10);
            engineSpeed(TX1, operand1, 5);
        } else if(!strcmp("q", cmd)){

        }


    }


}



